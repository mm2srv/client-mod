#include "music.hpp"

#include "../logging.hpp"

SETUP_FUNC_PTR(LockProxy_lock);
SETUP_FUNC_PTR(LockProxy_unlock);
SETUP_FUNC_PTR(SpinLock_lock);
SETUP_FUNC_PTR(SpinLock_unlock);

static inline auto GetSLinkSystem() {
  return exl::util::pointer_path::Follow<xlink2::System *, 0x02A36680>();
}

static inline auto GetSLinkSystemProxy() {
  // For locking
  return exl::util::pointer_path::Follow<xlink2::LockProxy *, 0x02A36688>();
}

void print_xlink_users() {
  auto users = GetSLinkSystem()->users;
  auto proxy = GetSLinkSystemProxy();

  LOG("Before lock", 0)
  LockProxy_lock(proxy);
  LOG("After lock", 0)

  int counter = 0;
  for (auto user : users) {
    LOG("User %d with name %s", counter, user.mName)
    counter++;
  }

  LOG("Before unlock", 0)
  LockProxy_unlock(proxy);
  LOG("After unlock", 0)
}

struct EventCallbackSLink : public xlink2::IEventCallbackSLink {
  int64_t eventActivating(EventArg const &) {
    LOG("eventActivating", 0)
    return 1;
  }
  uint64_t eventActivated(EventArg const &) {
    LOG("eventActivated", 0)
    return 1;
  }
  bool soundPrePlay(EventArg const &) {
    LOG("eventActivating", 0)
    return true;
  }
  void soundPlayed(EventArg const &) { LOG("eventActivating", 0) }
  void eventCalced(EventArg const &) { LOG("eventActivating", 0) }
  int replaceAssetInfo(aal::AssetInfo *, sead::SafeString const &str,
                       xlink2::UserInstanceSLink const *,
                       xlink2::ResAssetCallTable const *) {
    LOG("replaceAssetInfo %s", str.cstr())
    // 2 breaks instantly apparently
    return 1;
  }
};

struct OnEmitSound : public ::exl::hook::impl::TrampolineHook<OnEmitSound> {
  static uint64_t Callback(uintptr_t unk1) {
    // TODO
    // LOG("Sound emitted", 0)

    auto users = GetSLinkSystem()->users;
    int counter = 0;
    for (auto user : users) {
      if (counter == users.size())
        break;
      // LOG("User %d with name %s and num instances %d", counter, user.mName,
      //     user.userInstances.size())
      if (user.userInstances.size() > 0) {
        // Overload first event callback
        auto instance = user.userInstances.nth(0);
        if (instance->callback == nullptr) {
          instance->callback = new EventCallbackSLink();
        }
      }
      counter++;
    }

    return 1;
  }
};

void register_music() { OnEmitSound::InstallAtOffset(0x00596e10); }