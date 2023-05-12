#include "debug.hpp"

#include <wasm3/wasm3.h>

#include "../logging.hpp"
#include "../network/download.hpp"
#include "../state.hpp"

char *correct_user_name = "hello";

int loop_frame = 0;
u64 found_addr = 0;
struct DbgTextWriterDraw2D
    : public ::exl::hook::impl::ReplaceHook<DbgTextWriterDraw2D> {
  static void Callback(uintptr_t _this,
                       agl::lyr::RenderInfo const &renderInfo) {
    DbgTextWriter::Info info;

    info.mLayerId = renderInfo.mLayerId;
    info.mPos = {0, 350};
    info.mEx.mScale = {2, 2};
    info.mEx.mColor = {1.0, 1.0, 1.0, 1.0};
    info.mStrUtf8Data[0] = 0;

    /*
        if (loop_frame == 10000) {
          // In the level by now
          // uint8_t *bytes_to_look_for = (uint8_t *)malloc(8);
          // bytes_to_look_for[0] = 0x00;
          // bytes_to_look_for[1] = 0xCA;
          // bytes_to_look_for[2] = 0x9A;
          // bytes_to_look_for[3] = 0x3B;
          // bytes_to_look_for[4] = 0x00;
          // bytes_to_look_for[5] = 0x00;
          // bytes_to_look_for[6] = 0x00;
          // bytes_to_look_for[7] = 0x00;
          u64 addr = exl::util::mem_layout::s_Heap.m_Start + 0x97DA5CB + 1;
          while (addr < (exl::util::mem_layout::s_Heap.m_Start +
                         exl::util::mem_layout::s_Heap.m_Size - 8)) {
            // if (memcmp((void *)addr, bytes_to_look_for, 8) == 0) {
            //   found_addr = addr;
            //   break;
            // }
            if (*(u64 *)addr == 1000000000) {
              found_addr = addr;
              break;
            }
            addr += 1;
          }
          snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data),
                   "Found addr %ld %ld", found_addr,
                   exl::util::mem_layout::s_Heap.m_Start);
        } else if (loop_frame > 5000) {
          if (found_addr != 0) {
            snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data),
                     "Found addr %ld %ld", found_addr,
                     exl::util::mem_layout::s_Heap.m_Start);
          } else {
            snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data), "Close! %ld
       %ld", loop_frame, exl::util::mem_layout::s_Heap.m_Start);
          }
        } else {
          if (found_addr != 0) {
            snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data),
                     "Found addr %ld %ld", found_addr,
                     exl::util::mem_layout::s_Heap.m_Start);
          } else {
            snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data), "%ld %ld",
                     loop_frame, exl::util::mem_layout::s_Heap.m_Start);
          }
        }
        loop_frame++;
            */

    Lp_Sys_DbgTextWriter_drawText(_this, renderInfo, &info);
    // State::debug_draw_frame++;
  }
};

SETUP_FUNC_PTR(Lp_Sys_DbgTextWriter_draw2D);
SETUP_FUNC_PTR(Lp_Sys_DbgTextWriter_drawText);

void register_debug() {
  DbgTextWriterDraw2D::InstallAtFuncPtr(Lp_Sys_DbgTextWriter_draw2D);
}

// Code for finding heap values
/*
if (loop_frame == 12000) {
  // In the level by now
  uint8_t *bytes_to_look_for = (uint8_t *)malloc(8);

  bytes_to_look_for[0] = 0x3E;
  bytes_to_look_for[1] = 0xC8;
  bytes_to_look_for[2] = 0x2D;
  bytes_to_look_for[3] = 0x00;
  bytes_to_look_for[4] = 0x00;
  bytes_to_look_for[5] = 0x00;
  bytes_to_look_for[6] = 0x00;
  bytes_to_look_for[7] = 0x00;

  found_addr = 7;
  u64 addr = exl::util::mem_layout::s_Heap.m_Start;
  while (addr < (exl::util::mem_layout::s_Heap.m_Start +
                 exl::util::mem_layout::s_Heap.m_Size - 8)) {
    if (memcmp((void *)addr, bytes_to_look_for, 8) == 0) {
      found_addr = addr;
      break;
    }
    addr += 1;
  }

  snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data), "Found addr %ld
%ld", found_addr, exl::util::mem_layout::s_Heap.m_Start); } else if
(loop_frame > 5000) { if (found_addr != 0) { snprintf(info.mStrUtf8Data,
sizeof(info.mStrUtf8Data), "Found addr %ld %ld", found_addr,
exl::util::mem_layout::s_Heap.m_Start); } else {
snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data), "Close! %ld %ld",
loop_frame, exl::util::mem_layout::s_Heap.m_Start);
  }
} else {
  if (found_addr != 0) {
    snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data), "Found addr %ld
%ld", found_addr, exl::util::mem_layout::s_Heap.m_Start); } else {
    snprintf(info.mStrUtf8Data, sizeof(info.mStrUtf8Data), "%ld %ld",
             loop_frame, exl::util::mem_layout::s_Heap.m_Start);
  }
}
*/