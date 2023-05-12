#include <cstdint>

#include <wasm3/wasm3.h>

#include "../logging.hpp"
#include "../state.hpp"
#include "actor_modification.hpp"

struct BlockModelScale
    : public ::exl::hook::impl::TrampolineHook<BlockModelScale> {
  static void Callback(MapObjBlockBase *block, uintptr_t arg2,
                       BlockType blockType) {
    Orig(block, arg2, blockType);

    if (State::wasm_state.mod_change_block_model_scale &&
        State::wasm_state.mod_change_block_model_scale_memory) {
      // Both function and memory are available, call them
      State::wasm_state.mod_change_block_model_scale_memory[0] =
          block->mModelScale.x;
      State::wasm_state.mod_change_block_model_scale_memory[1] =
          block->mModelScale.y;
      State::wasm_state.mod_change_block_model_scale_memory[2] =
          block->mModelScale.z;
      auto base =
          (uint8_t *)State::wasm_state.mod_change_block_model_scale_memory -
          State::wasm_state.memory;
      if (m3_CallV(State::wasm_state.mod_change_block_model_scale,
                   block->mBlockType0x1, base, base + 4, base + 8) == 0) {
        block->mModelScale.x =
            State::wasm_state.mod_change_block_model_scale_memory[0];
        block->mModelScale.y =
            State::wasm_state.mod_change_block_model_scale_memory[1];
        block->mModelScale.z =
            State::wasm_state.mod_change_block_model_scale_memory[2];
      }
    }
  }
};

struct BlockModelPosition
    : public ::exl::hook::impl::TrampolineHook<BlockModelPosition> {
  static int Callback(MapObjBlockBase *block) {
    auto ret = Orig(block);

    if (State::wasm_state.mod_change_block_model_position &&
        State::wasm_state.mod_change_block_model_position_memory) {
      // Both function and memory are available, call them
      State::wasm_state.mod_change_block_model_position_memory[0] =
          block->mModelPosition.x;
      State::wasm_state.mod_change_block_model_position_memory[1] =
          block->mModelPosition.y;
      State::wasm_state.mod_change_block_model_position_memory[2] =
          block->mModelPosition.z;
      auto base =
          (uint8_t *)State::wasm_state.mod_change_block_model_position_memory -
          State::wasm_state.memory;
      if (m3_CallV(State::wasm_state.mod_change_block_model_position,
                   block->mBlockType0x1, base, base + 4, base + 8) == 0) {
        block->mModelPosition.x =
            State::wasm_state.mod_change_block_model_position_memory[0];
        block->mModelPosition.y =
            State::wasm_state.mod_change_block_model_position_memory[1];
        block->mModelPosition.z =
            State::wasm_state.mod_change_block_model_position_memory[2];
      }
    }

    return ret;
  }
};

void register_actor_modification() {
  BlockModelScale::InstallAtOffset(0x01385B00);
  BlockModelPosition::InstallAtOffset(0x01385EC0);
}