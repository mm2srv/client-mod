#pragma once

#include <cstdint>
#include <lib.hpp>
#include <math/seadVector.h>

enum BlockType : uint8_t {
  HardBlock = 0x6,
  KumoBlock = 0x66, // Seems to be cloud block (when you step on it)
  IceBlock = 0x78,
};
struct ActorBase {
  u8 mPadding0x1[0x18];
  sead::Vector3f mModelScale;
  u8 mPadding0x2[0x20C];
};

static_assert(sizeof(ActorBase) == 0x230, "ActorBase incorrect size");

struct Actor : public ActorBase {
  u8 mPadding0x1[0xc8];
  sead::Vector3f mModelPosition;
  uint8_t mPadding0x2[0xec];
};

static_assert(sizeof(Actor) == 0x3f0, "Actor incorrect size");

struct MapObjBlockBase : public Actor {
  BlockType mBlockType0x1;
  BlockType mBlockType0x2;
  u8 mPadding0x1[0x1bd];
};

static_assert(sizeof(MapObjBlockBase) == 0x5b0,
              "MapObjBlockBase incorrect size");

void register_actor_modification();