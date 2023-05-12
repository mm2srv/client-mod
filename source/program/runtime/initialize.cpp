#include "initialize.hpp"

#include "../api/patches.hpp"
#include "../state.hpp"
#include "runtime.hpp"

void initialize_mod(uint8_t *downloaded_data, size_t downloaded_size,
                    uint64_t data_id) {
  if (mod_exists()) {
    free_mod();
  }

  if (create_wasm_runtime(downloaded_data, downloaded_size,
                          &State::wasm_state.runtime, &State::wasm_state.env,
                          &State::wasm_state.mod) != WasmError::WasmNone) {
    // TODO
  }

  auto runtime = State::wasm_state.runtime;

  // Provide functions to the runtime
  m3_LinkRawFunction(State::wasm_state.mod, "env", "apply_patch", "v(II)",
                     &apply_patch);

  // Obtain memory
  State::wasm_state.memory = m3_GetMemory(runtime, nullptr, 0);

  // Check for start
  IM3Function mod_start;
  if (m3_FindFunction(&mod_start, runtime, "mod_start") == 0) {
    if (m3_CallV(mod_start, data_id)) {
      // TODO
    }
  }

  // Check for each function that might be used
  if (m3_FindFunction(&State::wasm_state.mod_malloc, runtime, "mod_malloc")) {
    State::wasm_state.mod_malloc = nullptr;
  }
  if (m3_FindFunction(&State::wasm_state.mod_name, runtime, "mod_name")) {
    State::wasm_state.mod_name = nullptr;
  }
  if (m3_FindFunction(&State::wasm_state.mod_change_block_model_scale, runtime,
                      "mod_change_block_model_scale")) {
    State::wasm_state.mod_change_block_model_scale = nullptr;
  } else {
    // Needs block of memory for 3 floats
    if (m3_CallV(State::wasm_state.mod_malloc, sizeof(float) * 3) == 0) {
      int malloc_pointer = 0;
      if (m3_GetResultsV(State::wasm_state.mod_malloc, &malloc_pointer) == 0) {
        State::wasm_state.mod_change_block_model_scale_memory =
            (float *)(State::wasm_state.memory + malloc_pointer);
      }
    }
  }
  if (m3_FindFunction(&State::wasm_state.mod_change_block_model_position,
                      runtime, "mod_change_block_model_position")) {
    State::wasm_state.mod_change_block_model_position = nullptr;
  } else {
    // Needs block of memory for 3 floats
    if (m3_CallV(State::wasm_state.mod_malloc, sizeof(float) * 3) == 0) {
      int malloc_pointer = 0;
      if (m3_GetResultsV(State::wasm_state.mod_malloc, &malloc_pointer) == 0) {
        State::wasm_state.mod_change_block_model_position_memory =
            (float *)(State::wasm_state.memory + malloc_pointer);
      }
    }
  }
}

bool mod_exists() { return State::wasm_state.runtime != 0; }

void free_mod() {
  // Deconstruct old runtime
  m3_FreeRuntime(State::wasm_state.runtime);
  // Deconstruct old environment
  m3_FreeEnvironment(State::wasm_state.env);
  // Revert all patches
  revert_patches();
  // Clear all state
  State::wasm_state = wasm_state_struct{};
}