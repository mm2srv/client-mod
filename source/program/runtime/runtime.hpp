#pragma once

#include <wasm3/wasm3.h>

enum WasmError {
  WasmNone,
  ParseError,
  LoadError,
};

WasmError create_wasm_runtime(uint8_t *wasm_bytes, size_t wasm_size,
                              IM3Runtime *runtime, IM3Environment *env,
                              IM3Module *mod);