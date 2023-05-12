#include "level.hpp"

#include <cstdio>
#include <cstdlib>
#include <curl/curl.h>
#include <util/sys/mem_layout.hpp>

#include "../func_pointers.hpp"
#include "../logging.hpp"
#include "../network/download.hpp"
#include "../runtime/initialize.hpp"
#include "../runtime/runtime.hpp"
#include "../setting.hpp"
#include "../state.hpp"

struct OnLevelLoadAndRestart
    : public ::exl::hook::impl::TrampolineHook<OnLevelLoadAndRestart> {
  static void Callback(int unk1, int unk2, int *unk3) {
    // Only works for non ninji levels
    uint64_t data_id =
        *(uint64_t *)(exl::util::mem_layout::s_Heap.m_Start + 0x52D69B0);
    // print_xlink_users();

    if (data_id != State::last_data_id) {
      if (data_id == 0) {
        // Player has started editing a level or returned to the home screen, do
        // not play mod
        // Also triggers for ninji levels, which have a different location for
        // the data id
        if (mod_exists()) {
          free_mod();
        }
      } else {
        // New online level has been loaded, download mod and initialize
        char url[15] = {0};
        snprintf(url, sizeof(url) - 1, "/mods/%ld", data_id);

        // Perform download
        DownloadError res;
        std::string downloaded_mod = download_from_url(url, &res);
        if (res != DownloadError::DownloadNone) {
          LOG("Mod downloading failed with: %d", res)
        } else {
          LOGN("Mod download was maybe successful")
        }
      }
    }
    State::last_data_id = data_id;

    Orig(unk1, unk2, unk3);
  }
};

struct OnDeath : public ::exl::hook::impl::TrampolineHook<OnDeath> {
  static void Callback(int64_t unk1) {
    // TODO
    Orig(unk1);
  }
};

void register_level() {
  OnLevelLoadAndRestart::InstallAtOffset(0x01792850);
  // OnDeath::InstallAtOffset(0x015B4810);
}