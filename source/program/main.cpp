#include <algorithm>
#include <atomic>
#include <cstring>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "api/actor_modification.hpp"
#include "api/debug.hpp"
#include "api/events.hpp"
#include "api/level.hpp"
#include "apply_token_and_patches.hpp"
#include "logging.hpp"

extern "C" void exl_main(void *x0, void *x1) {
  exl::hook::Initialize();

  apply_token_and_patches();
  register_actor_modification();
  register_debug();
  register_events();
  register_level();
  // register_music();
}

extern "C" NORETURN void exl_exception_entry() {
  /* TODO: exception handling */
  EXL_ABORT(0x420);
}