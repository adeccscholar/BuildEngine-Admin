# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

cmake_minimum_required(VERSION 3.25)
project(CmarkGfmSmoke LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(cmark-gfm CONFIG REQUIRED)

add_executable(CmarkGfmSmoke main.cpp)
set_property(TARGET CmarkGfmSmoke PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(CmarkGfmSmoke PRIVATE cmark-gfm::cmark-gfm-extensions)

add_custom_command(
   TARGET CmarkGfmSmoke POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:cmark-gfm::cmark-gfm>
      $<TARGET_FILE_DIR:CmarkGfmSmoke>
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:cmark-gfm::cmark-gfm-extensions>
      $<TARGET_FILE_DIR:CmarkGfmSmoke>
   VERBATIM
)
