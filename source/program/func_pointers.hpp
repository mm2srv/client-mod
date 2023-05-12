#pragma once

#include <lib.hpp>

#define _FUNC_PREFIX(name) slide::func_ptrs::name

#define _FUNC_TYPE(name) APPEND(_FUNC_PREFIX(name), _t)

#define _FUNC_OFFSET(name) APPEND(_FUNC_PREFIX(name), _Offset)

#define FUNC_PTR(offset, name, ...)                                            \
  namespace slide::func_ptrs {                                                 \
  using APPEND(name, _t) = __VA_ARGS__;                                        \
  static constexpr size_t APPEND(name, _Offset) = offset;                      \
  }                                                                            \
  extern _FUNC_TYPE(name) name

#define SETUP_FUNC_PTR(name)                                                   \
  _FUNC_TYPE(name)                                                             \
  name = reinterpret_cast<_FUNC_TYPE(name)>(                                   \
      exl::util::modules::GetTargetOffset(_FUNC_OFFSET(name)))
