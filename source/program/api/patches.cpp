#include "patches.hpp"

#include <lib.hpp>
#include <patch/random_access_patcher.hpp>
#include <wasm3/wasm3.h>

#include "../state.hpp"

void apply_patch(uintptr_t addr, uint64_t value) {
  exl::patch::RandomAccessPatcher p;
  if (!State::applied_patches_addr.count(addr)) {
    // Add the prior instruction at this address to revert the change later
    State::applied_patches.push_back(
        patch_struct{addr, p.Read<uint64_t>(addr)});
    State::applied_patches_addr.emplace(addr);
  }

  // Apply the patch
  p.Write(addr, value);

  // Flush changes
  p.Flush();
}

m3ApiRawFunction(apply_patch) {
  m3ApiGetArg(int64_t, addr);
  m3ApiGetArg(int64_t, value);

  apply_patch(addr, value);

  m3ApiSuccess();
}

void revert_patches() {
  exl::patch::RandomAccessPatcher p;
  for (auto &patch : State::applied_patches) {
    p.Write(patch.addr, patch.old_value);
  }
  p.Flush();

  State::applied_patches.clear();
  State::applied_patches_addr.clear();
}