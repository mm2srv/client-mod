#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum BlockType : uint8_t {
  HardBlock = 0x6,
  KumoBlock = 0x66, // Seems to be cloud block (when you step on it)
  IceBlock = 0x78,
};

// Functions called by host
// Called on startup
void mod_start(int data_id);
// Return name of the mod
const char *mod_name();
// Called once on the construction of a block actor
void mod_change_block_model_scale(uint8_t block, float *scale_x, float *scale_y,
                                  float *scale_z);
// Called once on the construction of a block actor, offset from its normal
// location
void mod_change_block_model_position(uint8_t block, float *x, float *y,
                                     float *z);
// Malloc memory in the mod
uint8_t *mod_malloc(int size);

// Functions webassembly can call
void apply_patch(int64_t addr, int64_t value);

#ifdef __cplusplus
}
#endif