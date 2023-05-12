#include "download.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <nn/nifm.h>
#include <nn/socket.h>
#include <stdlib.h>

#include "../logging.hpp"
#include "../setting.hpp"
#include "../state.hpp"

#define AF_UNSPEC 0
#define AF_INET 2
#define SOCK_STREAM 1

uint8_t *get_content_size(uint8_t *data, int *size, int *size_header) {
  // 2 newlines signals the end
  uint8_t *start_data =
      (uint8_t *)strstr((const char *)data, "\x0D\x0A\x0D\x0A") + 4;
  *size_header = (int)(start_data - data);

  // Iterate through tokens looking for Content-Length
  // This header must exist, this barebones http client doesn't support chunking
  // or a lack of this header
  auto loc = data;
  const char *content_length = "Content-Length: ";
  while (true) {
    if (memcmp(loc, content_length, strlen(content_length)) == 0) {
      uint8_t *number_loc = loc + strlen(content_length);
      uint8_t *end_loc;
      auto length = strtol((const char *)number_loc, (char **)&end_loc, 10);

      if (end_loc == number_loc) {
        // TODO, did not read correctly
        *size = -1;
      } else {
        *size = length;
      }
    }

    loc = (uint8_t *)strstr((const char *)loc, "\x0D\x0A") + 2;
    if (loc + 2 == start_data) {
      break;
    }
  }

  // Return start of the data
  return start_data;
}

std::string download_from_url(char *url, DownloadError *res) {
  if (nn::nifm::Initialize().IsFailure()) {
    *res = DownloadError::SocketCreationError;
    return "";
  }

  nn::nifm::SubmitNetworkRequestAndWait();

  while (nn::nifm::IsNetworkRequestOnHold()) {
  }

  auto socket = nn::socket::Socket(AF_INET, SOCK_STREAM, 0);
  if (socket == -1) {
    // nn::socket::GetLastErrno();
    *res = DownloadError::SocketCreationError;
    return "";
  }

  if (!State::serverIpAddr.s_addr) {
    // Obtain server IP address
    addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo *server_addr_info;
    int err = nn::socket::GetAddrInfo(State::domainBuffer, "http", &hints,
                                      &server_addr_info);
    if (err) {
      LOGN("Could not obtain IP address from server domain name")
      *res = DownloadError::SocketCreationError;
      return "";
    }
    // TODO construct proper struct
    State::serverIpAddr = *(in_addr *)(server_addr_info->ai_addr->sa_data + 2);
    LOG("IP for %s is %s", State::domainBuffer,
        nn::socket::InetNtoa(State::serverIpAddr))
  }

  sockaddr_in server_address;
  server_address.sin_addr = State::serverIpAddr;
  server_address.sin_family = AF_INET;
  server_address.sin_port = nn::socket::InetHtons(atoi(State::extraPortBuffer));

  int ret = nn::socket::Connect(socket, (struct sockaddr *)&server_address,
                                sizeof(server_address));
  if (ret < 0) {
    *res = DownloadError::SocketConnectionError;
    return "";
  }

  char request[211] = {0};
  // Copy URL into premade GET request
  snprintf(request, sizeof(request) - 1,
           "GET %s HTTP/1.0\r\nHost: "
           "%s\r\n\r\n Connection: "
           "keep-alive\r\n\r\n Keep-Alive: 300\r\n\r\n U: %s\r\n",
           url, State::domainBuffer, State::tokenHashBuffer);

  ret = nn::socket::Send(socket, request, strlen(request), 0);
  if (ret < 0) {
    nn::socket::Close(socket);
    *res = DownloadError::SocketHeaderError;
    return "";
  }

  uint8_t *server_reply = (uint8_t *)malloc(1000);
  uint8_t *data = nullptr;

  int total_len = 0;
  int size = 0;
  bool have_recieved_header = false;

  while (true) {
    if (size != 0 && total_len >= size) {
      // Have downloaded all data
      break;
    }

    int received_len = nn::socket::Recv(socket, server_reply, 1000, 0);

    if (received_len < 0) {
      // auto error = nn::socket::GetLastErrno();
      nn::socket::Close(socket);
      free(server_reply);
      free(data);
      *res = DownloadError::SocketRecvError;
      return "";
    } else if (received_len == 0) {
      if (total_len == 0) {
        // No bytes have been downloaded, probably 404
        *res = DownloadError::SocketNoBytes;
        return "";
      } else {
        // Orderly shutdown
        break;
      }
    }

    if (!have_recieved_header) {
      int size_header;
      auto data_start = get_content_size(server_reply, &size, &size_header);
      data = (uint8_t *)malloc(size);
      int size_data = std::min(received_len - size_header, size);

      // Copy data minus the header
      memcpy(data, data_start, size_data);
      total_len += size_data;
      have_recieved_header = true;
    } else {
      // Copy over new data
      memcpy(data + total_len, server_reply,
             std::min(received_len, size - total_len));
      total_len += received_len;
    }
  }

  free(server_reply);

  // Return downloaded data
  nn::socket::Close(socket);
  *res = DownloadError::DownloadNone;
  return std::string((const char *)data, size);
}