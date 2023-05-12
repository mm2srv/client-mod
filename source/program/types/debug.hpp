#pragma once

#include <lib.hpp>

#include <sead/container/seadTreeNode.h>
#include <sead/gfx/seadCamera.h>
#include <sead/gfx/seadColor.h>
#include <sead/gfx/seadDrawContext.h>
#include <sead/gfx/seadProjection.h>
#include <sead/math/seadBoundBox.hpp>
#include <sead/math/seadMatrix.hpp>
#include <sead/prim/seadSafeString.hpp>

#include "vtbl.hpp"

namespace agl::lyr {

struct RenderInfo {
  uint field_0;
  uint field_4;
  bool field_8;
  uintptr_t mRenderBuffer;
  uintptr_t mLayer;
  int mLayerId;
  sead::Camera *mRenderCamera;
  sead::Projection *mRenderProjection;
  uintptr_t mViewport;
  bool field_40;
  uintptr_t *mDrawContext;
};
} // namespace agl::lyr

namespace DbgTextWriter {

struct ArgEx {
  int field_0 = 1;
  int field_4 = 1;
  sead::Vector2f mScale;
  float field_10 = 0;
  sead::Color4f mColor;
  uint mFlags = 0;
  bool mIsDecDrawFrameInScenePause = true;
  bool mIsDecDrawFrameDelayTrigInScenePause = false;

  ArgEx() : mScale(1, 1), mColor(1, 1, 1, 1) {}
};

struct Info {
  static constexpr s32 BufferSize = 256;

  int mLayerId;
  u8 field_4;
  sead::BufferedSafeString mStrUtf8;
  sead::WBufferedSafeString mStrUtf16;
  union {
    char mStrUtf8Data[BufferSize / sizeof(char)];
    char16_t mStrUtf16Data[BufferSize / sizeof(char16_t)];
  };
  sead::Vector2f mPos;
  ArgEx mEx;

  Info()
      : mStrUtf8(mStrUtf8Data, sizeof(mStrUtf8Data) / sizeof(char)),
        mStrUtf16(mStrUtf16Data, sizeof(mStrUtf16Data) / sizeof(char16_t)) {
    /* Horrible hack. */
    VTBL_WRAP(strUtf8, &mStrUtf8);
    VTBL_WRAP(strUtf16, &mStrUtf16);

    strUtf8->m_Vtbl = exl::util::modules::GetTargetOffset(0x027B8140);
    strUtf16->m_Vtbl = exl::util::modules::GetTargetOffset(0x027E95F8);
  }
};
} // namespace DbgTextWriter
