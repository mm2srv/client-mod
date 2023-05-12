#include <cstdint>

#define M3_COMPILE_OPCODES
#include <wasm3/m3_exec.h>

#include "../state.hpp"
#include "runtime.hpp"

m3ApiRawFunction(change_block_scale) {
  // TODO
  m3ApiSuccess();
}

WasmError create_wasm_runtime(uint8_t *wasm_bytes, size_t wasm_size,
                              IM3Runtime *runtime, IM3Environment *env,
                              IM3Module *mod) {
  constexpr int WASM_STACK_SIZE = 2048;
  constexpr int WASM_MEMORY_LIMIT = 0x10000;

  M3Result result = m3Err_none;
  *env = m3_NewEnvironment();
  *runtime = m3_NewRuntime(*env, WASM_STACK_SIZE, NULL);
  (*runtime)->memoryLimit = WASM_MEMORY_LIMIT;

  result = m3_ParseModule(*env, mod, wasm_bytes, wasm_size);
  if (result) {
    return WasmError::ParseError;
  }

  result = m3_LoadModule(*runtime, *mod);
  if (result) {
    return WasmError::LoadError;
  }

  return WasmError::WasmNone;
}