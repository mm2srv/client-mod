#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <nn/socket.h>
#include <unordered_set>
#include <vector>
#include <wasm3/wasm3.h>

struct wasm_state_struct {
  IM3Runtime runtime = nullptr;
  IM3Environment env = nullptr;
  IM3Module mod = nullptr;
  M3Result error;
  // Functions
  uint8_t *memory = nullptr;
  IM3Function mod_malloc = nullptr;
  IM3Function mod_name = nullptr;
  IM3Function mod_change_block_model_scale = nullptr;
  float *mod_change_block_model_scale_memory = nullptr;
  IM3Function mod_change_block_model_position = nullptr;
  float *mod_change_block_model_position_memory = nullptr;
};

struct patch_struct {
  uintptr_t addr;
  uint64_t old_value;
};

namespace State {
// Domain and auth handling
extern char tokenBuffer[100];
extern char domainBuffer[100];
extern char domainPortBuffer[6];
extern char extraPortBuffer[6];

extern in_addr serverIpAddr;

// Token hash buffer for requests
extern char tokenHashBuffer[33];

// Last data id
extern uint64_t last_data_id;

// Wasm handling
extern wasm_state_struct wasm_state;

// Mod applied
extern std::vector<patch_struct> applied_patches;
extern std::unordered_set<uintptr_t> applied_patches_addr;
} // namespace State