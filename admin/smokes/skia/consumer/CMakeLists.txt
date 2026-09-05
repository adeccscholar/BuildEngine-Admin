# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeSkia LANGUAGES CXX)

find_package(Skia CONFIG REQUIRED)

add_executable(skia-consumer main.cpp)
set_property(TARGET skia-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(skia-consumer PRIVATE cxx_std_20)
target_compile_definitions(skia-consumer PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(skia-consumer PRIVATE Skia::skia)

add_custom_command(TARGET skia-consumer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:Skia::skia>
      $<TARGET_FILE_DIR:skia-consumer>
   VERBATIM)
