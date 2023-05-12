#pragma once

#include <cstdint>
#include <lib.hpp>

#include "../func_pointers.hpp"
#include "../types/debug.hpp"


FUNC_PTR(0x0062E1A0, Lp_Sys_DbgTextWriter_draw2D,
         void (*)(uintptr_t, agl::lyr::RenderInfo const &));
FUNC_PTR(0x0062DCC0, Lp_Sys_DbgTextWriter_drawText,
         void (*)(uintptr_t, agl::lyr::RenderInfo const &,
                  DbgTextWriter::Info *));

void register_debug();