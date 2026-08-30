# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar
#
# Activated only after a BoostConfig.cmake has actually been published into the
# central Win64x SDK.  This avoids changing compiler classification for an
# unrelated RAD Studio / BDS Boost provider.

file(GLOB_RECURSE _ADECC_BOOST_CONFIGS LIST_DIRECTORIES FALSE
   "${ADECC_BUILDENGINE_SDK_ROOT}/lib/cmake/BoostConfig.cmake"
   "${ADECC_BUILDENGINE_SDK_ROOT}/lib/cmake/*/BoostConfig.cmake")
if(_ADECC_BOOST_CONFIGS)
   # The managed Boost 1.92 headers must also precede RAD Studio's bundled Boost provider.
   include_directories(BEFORE "${ADECC_BUILDENGINE_SDK_ROOT}/include")
   include("${CMAKE_CURRENT_LIST_DIR}/bcc64x-native-clang/project-include.cmake")
   set(ADECC_BUILDENGINE_BOOST_192_AVAILABLE TRUE)
endif()
unset(_ADECC_BOOST_CONFIGS)
