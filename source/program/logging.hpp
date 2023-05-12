#pragma once

#include <nn/diag.h>

namespace nn {
namespace diag {
enum LogSeverity {
  LogSeverity_Trace,
  LogSeverity_Info,
  LogSeverity_Warn,
  LogSeverity_Error,
  LogSeverity_Fatal
};

struct SourceInfo {
  int lineNumber;
  const char *fileName;
  const char *functionName;
};

struct LogMetaData {
  SourceInfo sourceInfo;
  const char *moduleName;
  LogSeverity severity;
  int verbosity;
  bool useDefaultLocaleCharset;
  void *pAdditionalData;
  size_t additionalDataBytes;
};
} // namespace diag
} // namespace nn

// LOG with args
#define LOG(str, ...)                                                          \
  {                                                                            \
    nn::diag::LogMetaData logMeta;                                             \
    logMeta.severity = nn::diag::LogSeverity_Info;                             \
    logMeta.sourceInfo.lineNumber = __LINE__;                                  \
    logMeta.sourceInfo.fileName = __FILE__;                                    \
    logMeta.sourceInfo.functionName = __FUNCTION__;                            \
    logMeta.moduleName = "ModLoader";                                          \
    nn::diag::detail::LogImpl(logMeta, str, __VA_ARGS__);                      \
  }

// LOG with no args
#define LOGN(str)                                                              \
  {                                                                            \
    nn::diag::LogMetaData logMeta;                                             \
    logMeta.severity = nn::diag::LogSeverity_Info;                             \
    logMeta.sourceInfo.lineNumber = __LINE__;                                  \
    logMeta.sourceInfo.fileName = __FILE__;                                    \
    logMeta.sourceInfo.functionName = __FUNCTION__;                            \
    logMeta.moduleName = "ModLoader";                                          \
    nn::diag::detail::LogImpl(logMeta, str);                                   \
  }
