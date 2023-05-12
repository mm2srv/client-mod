#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <common.hpp>
#include <sead/codec/seadHashCRC32.h>
#include <sead/container/seadOffsetList.h>
#include <sead/thread/seadSpinLock.h>
#include <util/modules.hpp>
#include <util/ptr_path.hpp>

#include "../func_pointers.hpp"

namespace aal {
namespace AudioMeta {
// TODO
using Bin = int;
} // namespace AudioMeta

// Not neccesary right now
using Handle = int;

struct AudioMetaReader {
  // TODO
  uint64_t unk1;
  aal::AudioMeta::Bin *bin;
};

struct AssetInfo {
  uint64_t unk1;
  aal::AudioMetaReader reader;
};
} // namespace aal

namespace xlink2 {
// Not neccesary right now
using ResAssetCallTable = int;
using AssetExecutorSLink = int;
using EventSLink = int;

struct UserInstanceSLink;
struct IEventCallbackSLink;
struct User;
struct System;

struct UserInstanceSLink {
  uint8_t unk1[0x110];
  xlink2::IEventCallbackSLink *callback;
};

struct IEventCallbackSLink {
  struct EventArg {
    xlink2::UserInstanceSLink *mUserInterface;
    aal::Handle *mAalHandle;
    xlink2::ResAssetCallTable *mResAssetCallTable;
    xlink2::AssetExecutorSLink *mExecutor;
    xlink2::EventSLink *mEvent;
  };

  virtual int64_t eventActivating(EventArg const &) = 0;
  virtual uint64_t eventActivated(EventArg const &) = 0;
  virtual bool soundPrePlay(EventArg const &) = 0;
  virtual void soundPlayed(EventArg const &) = 0;
  virtual void eventCalced(EventArg const &) = 0;
  virtual int replaceAssetInfo(aal::AssetInfo *, sead::SafeString const &,
                               UserInstanceSLink const *,
                               ResAssetCallTable const *) = 0;
};

struct User {
  uint64_t unk1;
  uint64_t unk2;
  const char *mName;
  uint64_t unk3;
  sead::OffsetList<xlink2::UserInstanceSLink> userInstances;
};

struct LockProxy {
  sead::SpinLock *spinLock;
  sead::SpinLock *spinLock2;
};

struct System {
  uint8_t unk1[0x10];
  sead::OffsetList<xlink2::User> users;
  // TODO more after this
};
} // namespace xlink2

FUNC_PTR(0x005947f0, LockProxy_lock, void (*)(xlink2::LockProxy *));
FUNC_PTR(0x00594800, LockProxy_unlock, void (*)(xlink2::LockProxy *));
FUNC_PTR(0x00293370, SpinLock_lock, void (*)(sead::SpinLock *));
FUNC_PTR(0x00293420, SpinLock_unlock, void (*)(sead::SpinLock *));

void register_music();
void print_xlink_users();