#include "lib.hpp"
#include <nn.hpp>
#include <string>
#include "util/file_handle.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

#define LOG(...)                                            \
  {                                                         \
      int length = snprintf(buf, sizeof(buf), __VA_ARGS__); \
      svcOutputDebugString(buf, length);                    \
  }

/* Utility to see if a file exists. */
static bool FileExists(const char* path) {
    nn::fs::DirectoryEntryType type;
    auto r = nn::fs::GetEntryType(&type, path);
    if(R_FAILED(r))
        return false;
    else
        return type == nn::fs::DirectoryEntryType_File;
}

/* File path to read the token from. */
static constexpr char s_ConfigPath[] = "sd:/ocw-config.json";

static constexpr size_t s_MaxJsonSize = 2048;
static std::array<char, s_MaxJsonSize> s_JsonBuffer {};

static constexpr size_t s_MaxDomainPatchBufferSize = 29;
static std::array<char, s_MaxDomainPatchBufferSize> s_DomainPatchBuffer {};

static constexpr size_t s_TokenSize = 22;
static std::array<char, s_TokenSize + 1> s_TokenPatchBuffer {};

void HeaderSnprintf(char* buffer, size_t bufferLength, const char* format, const char* path, const char* host, const char* key, const char* protocol) {
    snprintf(buffer, bufferLength,
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Upgrade: websocket\r\n"
        "Connection: upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "%s: %s\r\n"
        "\r\n",
        path,
        host,
        key,
        protocol,
        s_TokenPatchBuffer.data()
    );
}

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    exl::hook::Initialize();

    /* Log line buffer. */
    char buf[500];

    LOG("OCW: reading sdcard://ocw-config.json");
    
    /* Mount the SD card, this must succeeed. */
    R_ABORT_UNLESS(nn::fs::MountSdCardForDebug("sd"));
    
    /* Do nothing if the token is missing. */
    if(!FileExists(s_ConfigPath)) {
        LOG("OCW: sdcard://ocw-config.json file missing, abort..");
        EXL_ABORT(0x420);
        return;
    }

    /* Open config file and validate it's size. */
    util::FileHandle handle(s_ConfigPath, nn::fs::OpenMode_Read);
    auto fileSize = handle.GetSize();
    if(fileSize > s_MaxJsonSize) {
        LOG("OCW: config abnormally large, abort...");
        EXL_ABORT(0x420);
        return;
    }

    LOG("OCW: parsing sdcard://ocw-config.json");

    /* Read entire token file. */
    handle.Read(std::span {s_JsonBuffer.begin(), s_JsonBuffer.begin() + fileSize});
    json ocw_config = json::parse(s_JsonBuffer.begin(), s_JsonBuffer.end());

    if (ocw_config.contains("domain") != true) {
        LOG("OCW: domain missing, abort...");
        EXL_ABORT(0x420);
    }
    std::string _domainValue = ocw_config["domain"].get<std::string>();
    if (_domainValue.length() > s_MaxDomainPatchBufferSize) {
        LOG("OCW: domain is too long, abort...");
        EXL_ABORT(0x420);
        return;
    }
    std::copy(_domainValue.begin(), _domainValue.end(), s_DomainPatchBuffer.data());

    if (ocw_config.contains("token") != true) {
        LOG("OCW: token missing, abort...");
        EXL_ABORT(0x420);
    }
    std::string _tokenValue = ocw_config["token"].get<std::string>();
    if (_tokenValue.length() != s_TokenSize) {
        LOG("OCW: token invalid length, abort...");
        EXL_ABORT(0x420);
        return;
    }
    std::copy(_tokenValue.begin(), _tokenValue.end(), s_TokenPatchBuffer.data());


    LOG("OCW: applying patches");
    /* Patch in our domain. */
    exl::patch::CodePatcher p(0x22cc307);
    p.Write(s_DomainPatchBuffer.data(), s_MaxDomainPatchBufferSize);

    /* Hook snprintf call that builds the HTTP headers for requests to place in the token. */
    p.Seek(0xAF570);
    p.BranchLinkInst((void*) HeaderSnprintf);

    /* Stub GetNetworkServiceAccountId, returning always zero and 0xCAFE as the account id. */
    p.Seek(0x1cb75e0);
    p.WriteInst(inst::Movz(reg::X8, 0xCAFE));
    p.WriteInst(inst::StrRegisterImmediate(reg::X8, reg::X0));
    p.WriteInst(inst::Movz(reg::W0, 0));
    p.WriteInst(inst::Ret());

    /* Stub IsNetworkServiceAccountAvailable, returning always zero. */
    p.Seek(0x1cba060);
    p.WriteInst(inst::Movz(reg::W0, 0));
    p.WriteInst(inst::Ret());

    /* Stub LoadNetworkServiceAccountIdTokenCache , returning always zero. */
    p.Seek(0x1cb75b0);
    p.WriteInst(inst::Movz(reg::W0, 0));
    p.WriteInst(inst::Ret());

    /* Disable profanity filter. */
    p.Seek(0x17b7fe0);
    p.WriteInst(inst::Movz(reg::W0, 1));
    p.WriteInst(inst::Ret());

    /* Disable corrupt course check. */
    p.Seek(0x17bb980);
    p.WriteInst(inst::Movz(reg::W0, 1));
    p.WriteInst(inst::Ret());

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

    LOG("OCW: finished");
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}
