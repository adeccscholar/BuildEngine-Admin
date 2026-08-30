# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar
include_guard(GLOBAL)
if(DEFINED ADECC_BOOST_MANAGED_INCLUDE_DIR AND EXISTS "${ADECC_BOOST_MANAGED_INCLUDE_DIR}/boost/version.hpp")
   include_directories(BEFORE "${ADECC_BOOST_MANAGED_INCLUDE_DIR}")
endif()
include_directories(BEFORE "${CMAKE_CURRENT_LIST_DIR}")
