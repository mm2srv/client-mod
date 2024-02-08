#pragma once

#include "lib.hpp"
#include "nn.hpp"

namespace helpers
{
    const char GAME_VERSION_3_0_2[] = "3.0.2";
    const char GAME_VERSION_3_0_3[] = "3.0.3";

    extern char sGameVersion[0x10];
    extern u32  sGameVersionIndex;

    u32 InitializeGameVersion();
    ALWAYS_INLINE char* GetGameVersion() { return &sGameVersion[0]; }
}
