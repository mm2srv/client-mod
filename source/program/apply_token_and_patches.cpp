#include "setting.hpp"

#define INFO_FILE_PATH "sd:/ocw-info.txt"

#include <array>
#include <cstring>
#include <nn/fs.h>
#include <nn/socket.h>
#include <patch/code_patcher.hpp>

#include "logging.hpp"
#include "state.hpp"
#include "third_party/md5.hpp"

static bool FileExists(const char *path) {
  nn::fs::DirectoryEntryType type;
  auto r = nn::fs::GetEntryType(&type, path);
  if (r.IsFailure())
    return false;
  else
    return type == nn::fs::DirectoryEntryType_File;
}

void build_hex_string(char *str, uint8_t *data, size_t data_size) {
  constexpr char *hex = "0123456789ABCDEF";
  for (int i = 0; i < data_size; i++) {
    *str++ = hex[(*data >> 4) & 0xF];
    *str++ = hex[(*data++) & 0xF];
  }
  // Null byte
  *str = 0;
}

void HeaderSnprintf(char *buffer, size_t bufferLength, const char *format,
                    const char *path, const char *host, const char *key,
                    const char *protocol) {
  snprintf(buffer, bufferLength,
           "GET %s HTTP/1.1\r\n"
           "Host: %s\r\n"
           "Upgrade: websocket\r\n"
           "Connection: upgrade\r\n"
           "Sec-WebSocket-Key: %s\r\n"
           "Sec-WebSocket-Version: 13\r\n"
           "%s: %s\r\n"
           "\r\n",
           path, host, key, protocol, State::tokenBuffer);
}

void apply_token_and_patches() {
  LOGN("Starting OCW patching")

  if (nn::fs::MountSdCardForDebug("sd").IsFailure()) {
    LOGN("Could not mount sd card, skipping patching")
    return;
  }

  // Check for existence of info file
  if (!FileExists(INFO_FILE_PATH)) {
    LOGN("Domain file missing, skipping patching")
    return;
  }

  // Open info file
  nn::fs::FileHandle fileHandle;
  if (nn::fs::OpenFile(&fileHandle, INFO_FILE_PATH, nn::fs::OpenMode_Read)
          .IsFailure()) {
    LOGN("Could not open domain file, skipping patching")
    return;
  }

  // Validate domain file size
  s64 fileSize;
  nn::fs::GetFileSize(&fileSize, fileHandle);
  s64 const maxFileSize =
      sizeof(State::domainBuffer) + sizeof(State::tokenBuffer) +
      sizeof(State::domainPortBuffer) + sizeof(State::extraPortBuffer);
  if (fileSize > maxFileSize) {
    LOGN("Domain file abnormally large, skipping patching")
    return;
  }

  // Read entire info file
  char domainFileContents[maxFileSize] = {0};
  if (nn::fs::ReadFile(fileHandle, 0, domainFileContents, fileSize)
          .IsFailure()) {
    LOGN("Could not read domain file, skipping patching")
    return;
  }

  // Check for each value by splitting on newlines
  char *currentPos = domainFileContents;
  char *buffers[] = {State::domainBuffer, State::tokenBuffer,
                     State::domainPortBuffer, State::extraPortBuffer};
  for (int i = 0; i < 5; i++) {
    char *nextPos = strchr(currentPos, '\n');
    if (nextPos == NULL && nextPos != currentPos) {
      // Last line and this is not a newline then end of file
      strcpy(buffers[i], currentPos);
      break;
    } else {
      // Newline was found, copy only the text between currentPos and nextPos
      memcpy((void *)buffers[i], currentPos, nextPos - currentPos);
      // Set currentPos to after the newline
      currentPos = nextPos + 1;
    }
  }

  if (State::domainBuffer[0] == 0) {
    // Default OpenCourseWorld domain
    strcpy(State::domainBuffer, "server.opencourse.world");
  }
  if (State::tokenBuffer[0] == 0) {
    // ReadOnly token
    strcpy(State::tokenBuffer, "d4LtgY4xDecIBBUUPVw8Vw");
  }
  if (State::domainPortBuffer[0] == 0) {
    // Default OpenCourseWorld main port
    strcpy(State::domainPortBuffer, "443");
  }
  if (State::extraPortBuffer[0] == 0) {
    // Default OpenCourseWorld extra port
    strcpy(State::extraPortBuffer, "6001");
  }

  // Get MD5 hash of token
  uint8_t tokenHash[16];
  md5String(State::tokenBuffer, tokenHash);

  // Turn the hash into a hexidecimal string
  build_hex_string(State::tokenHashBuffer, tokenHash, sizeof(tokenHash));

  namespace inst = exl::armv8::inst;
  namespace reg = exl::armv8::reg;

  // Patch in our domain and port
  exl::patch::CodePatcher p(0x22cc307);
  p.Write(State::domainBuffer, strlen(State::domainBuffer));
  p.Write(":", 1);
  p.Write(State::domainPortBuffer, strlen(State::domainPortBuffer) + 1);

  // Hook snprintf call that builds the HTTP headers for auth token
  p.Seek(0xAF570);
  p.BranchLinkInst((void *)HeaderSnprintf);

  // Stub GetNetworkServiceAccountId, returning always zero
  // and 0xCAFE as the account id
  p.Seek(0x1cb75e0);
  p.Write(inst::Movz(reg::X8, 0xCAFE));
  p.Write(inst::StrRegisterImmediate(reg::X8, reg::X0));
  p.Write(inst::Movz(reg::W0, 0));
  p.Write(inst::Ret());

  // Stub IsNetworkServiceAccountAvailable, returning always zero
  p.Seek(0x1cba060);
  p.Write(inst::Movz(reg::W0, 0));
  p.Write(inst::Ret());

  // Stub LoadNetworkServiceAccountIdTokenCache , returning always zero
  p.Seek(0x1cb75b0);
  p.Write(inst::Movz(reg::W0, 0));
  p.Write(inst::Ret());

  // Disable profanity filter
  p.Seek(0x17b7fe0);
  p.Write(inst::Movz(reg::W0, 1));
  p.Write(inst::Ret());

  // Disable corrupt course check
  p.Seek(0x17bb980);
  p.Write(inst::Movz(reg::W0, 1));
  p.Write(inst::Ret());

  /* Pretend EnsureNetworkServiceAccountIdTokenCacheAsync succeeds. */
  p.Seek(0x01CB7560);
  p.WriteInst(inst::Movz(reg::W0, 0));
  p.WriteInst(inst::Ret());

  /* Skip over the game trying to use the AsyncContext to wait on the token cache init to finish. Just pretend it succeeded. */
  p.Seek(0x0002F44C);
  p.WriteInst(inst::Movz(reg::W0, 0));
  p.BranchInst(0x0002F464);

  /* Then skip over deconstructing the AsyncContext. */
  p.Seek(0x0002F548);
  p.WriteInst(inst::Nop());
  
  /* Stub EnsureNetworkServiceAccountAvailable, returning always zero. */
  p.Seek(0x0060E7A4);
  p.WriteInst(inst::Movz(reg::W0, 0));
}
