# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

# BCC64X/CMake currently supplies a Unix-like static-library prefix for these
# upstream targets. Keep upstream GoogleTest untouched and normalize only the
# managed Windows/BCC64X package artifacts to the frozen contract:
#   gtest.lib, gtest_main.lib, gmock.lib, gmock_main.lib
function(AdeccFixGoogleTestTargetProperties)
   foreach(theTarget IN ITEMS gtest gtest_main gmock gmock_main)
      if(TARGET ${theTarget})
         get_target_property(theTargetType ${theTarget} TYPE)
         if(theTargetType STREQUAL "STATIC_LIBRARY")
            set_property(TARGET ${theTarget} PROPERTY PREFIX "")
         endif()
      endif()
   endforeach()
endfunction()

# CMAKE_PROJECT_INCLUDE loads this file during the upstream project() call.
# Defer until GoogleTest/GoogleMock have created all four static targets.
cmake_language(DEFER CALL AdeccFixGoogleTestTargetProperties)
