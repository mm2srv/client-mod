#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

enum DownloadError {
  DownloadNone,
  SocketCreationError,
  SocketConnectionError,
  SocketHeaderError,
  SocketRecvError,
  SocketNoBytes,
};

// Don't pass the port or domain, those are loaded from the file
std::string download_from_url(char *url, DownloadError *res);
