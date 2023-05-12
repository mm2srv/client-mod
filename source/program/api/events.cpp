#include "events.hpp"
#include "../logging.hpp"

struct DeathSignal : public ::exl::hook::impl::TrampolineHook<DeathSignal> {
  static void Callback(uintptr_t _this) {
    // TODO
    Orig(_this);
  }
};

void register_events() { DeathSignal::InstallAtOffset(0x015B4810); }