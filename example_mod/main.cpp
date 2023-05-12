#include "../mod_headers/mod.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

void mod_start(int data_id) {
  apply_patch(0x01BCDDFC + 0x60, 0x68008052);
  apply_patch(0x01BCE720 + 0x60, 0x68008052);
  apply_patch(0x0190F27C + 0x60, 0xA80080D2);
  apply_patch(0x0190F280 + 0x60, 0xE8FFDFF2);
  apply_patch(0x0192042C + 0x60, 0x087D8052);
  apply_patch(0x019204E4 + 0x60, 0x087D8052);
  apply_patch(0x01920628 + 0x60, 0xA8008052);
  apply_patch(0x01920634 + 0x60, 0xA8008052);
  apply_patch(0x013C50A8 + 0x60, 0x0190221E);
  apply_patch(0x013C5294 + 0x60, 0x08008052);
  apply_patch(0x013C5CBC + 0x60, 0x01103C1E);
  apply_patch(0x013C5CC0 + 0x60, 0x0008211E);
  apply_patch(0x013C5CC4 + 0x60, 0x1F2003D5);
  apply_patch(0x013C5CC8 + 0x60, 0x1F2003D5);
  apply_patch(0x013C5D44 + 0x60, 0x01103C1E);
  apply_patch(0x013C5D48 + 0x60, 0x0008211E);
  apply_patch(0x013C5D4C + 0x60, 0x1F2003D5);
  apply_patch(0x013C5D50 + 0x60, 0x1F2003D5);
  apply_patch(0x023205F4 + 0x60, 0x00000000);
  apply_patch(0x023205F8 + 0x60, 0x00000000);
  apply_patch(0x023205FC + 0x60, 0x00000000);
  apply_patch(0x00AFD0EC + 0x60, 0x0A22A8D2);
  apply_patch(0x013ABAC8 + 0x60, 0x0010201E);
  apply_patch(0x00B0053C + 0x60, 0x0A22A8D2);
  apply_patch(0x023204A0 + 0x60, 0x000080BD);
  apply_patch(0x023204A4 + 0x60, 0x0000803D);
  apply_patch(0x023204A8 + 0x60, 0x000080BD);
  apply_patch(0x023204AC + 0x60, 0x00000000);
  apply_patch(0x00EE1504 + 0x60, 0x0820A852);
  apply_patch(0x00EE1CBC + 0x60, 0x1110241E);
  apply_patch(0x00EE1DE0 + 0x60, 0x0410341E);
  apply_patch(0x013B1B68 + 0x60, 0x0D20A852);
  apply_patch(0x013B1BF0 + 0x60, 0x0820B852);
  apply_patch(0x013B1BF8 + 0x60, 0x1520E8D2);
  apply_patch(0x013B1D88 + 0x60, 0x0010201E);
  apply_patch(0x013B20B0 + 0x60, 0x0820E8D2);
  apply_patch(0x013B1FC4 + 0x60, 0x1F2003D5);
  apply_patch(0x013B20C0 + 0x60, 0x1F2003D5);
  apply_patch(0x02320880 + 0x60, 0x00008041);
  apply_patch(0x02320888 + 0x60, 0x000080C1);
  apply_patch(0x02320DB4 + 0x60, 0x000080C1);
  apply_patch(0x02320DBC + 0x60, 0x00008041);
  apply_patch(0x0138C8D8 + 0x60, 0x00102C1E);
  apply_patch(0x0138C8F8 + 0x60, 0x0110291E);
  apply_patch(0x0138C904 + 0x60, 0x02102F1E);
  apply_patch(0x01117A20 + 0x60, 0x0D10261E);
  apply_patch(0x01117B58 + 0x60, 0x0110261E);
  apply_patch(0x01117AD0 + 0x60, 0x1F2003D5);
  apply_patch(0x01117A18 + 0x60, 0x0A102C1E);
  apply_patch(0x011191EC + 0x60, 0x0810201E);
}

extern "C" {
__attribute__((used)) const char *mod_name() { return "Awesome test mod"; }

__attribute__((used)) void mod_change_block_model_scale(uint8_t block,
                                                        float *scale_x,
                                                        float *scale_y,
                                                        float *scale_z) {
  //*scale_x = 1.0;
  //*scale_y = 1.0;
  //*scale_z = 1.0;
}

__attribute__((used)) void
mod_change_block_model_position(uint8_t block, float *x, float *y, float *z) {
  // *x = 0.0;
  // *y = 0.0;
}

// Required for some functions like mod_change_block_model_scale

// Using a custom malloc implementation
// Traditional malloc will raise a trapOutOfBoundsMemoryAccess in wasm3
uint8_t malloc_memory[1000];
int malloc_memory_loc = 0;

__attribute__((used)) uint8_t *mod_malloc(int size) {
  auto mem = &malloc_memory[malloc_memory_loc];
  malloc_memory_loc += size;
  return mem;
}
}