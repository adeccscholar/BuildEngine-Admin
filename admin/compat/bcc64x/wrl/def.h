// BuildEngine compatibility overlay for the Microsoft WRL version gate.
//
// BCC64X uses the native Windows C++ ABI but does not define _MSC_VER.
// Microsoft's WRL/def.h uses _MSC_VER only as a minimum-version gate;
// keep the compatibility value scoped strictly to that header inclusion.
// Never propagate the value into the project or CMake/Meson compile flags.
#pragma once

#if defined(__CODEGEARC__) || defined(__BORLANDC__)
#  if !defined(_MSC_VER)
#    define _MSC_VER 1600
#    define ADECC_WRL_RESTORE_MSC_VER 1
#  endif
#endif

// Clang's include_next resolves to the original installed Windows SDK header,
// which remains the authoritative WRL implementation.
#include_next <wrl/def.h>

#if defined(ADECC_WRL_RESTORE_MSC_VER)
#  undef _MSC_VER
#  undef ADECC_WRL_RESTORE_MSC_VER
#endif
