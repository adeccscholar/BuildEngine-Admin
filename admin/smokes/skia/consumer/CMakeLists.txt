# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeSkia LANGUAGES CXX)

find_package(Skia CONFIG REQUIRED)

add_executable(skia-consumer main.cpp)
set_property(TARGET skia-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(skia-consumer PRIVATE cxx_std_23)
target_compile_definitions(skia-consumer PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(skia-consumer PRIVATE
   Skia::skia
   Skia::skunicode
   Skia::skshaper
   Skia::svg
   Skia::skottie
)

add_custom_command(TARGET skia-consumer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:Skia::skia>
      $<TARGET_FILE_DIR:skia-consumer>
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:Skia::skunicode_core>
      $<TARGET_FILE_DIR:skia-consumer>
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:Skia::skunicode_icu>
      $<TARGET_FILE_DIR:skia-consumer>
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:Skia::skshaper>
      $<TARGET_FILE_DIR:skia-consumer>
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:Skia::svg>
      $<TARGET_FILE_DIR:skia-consumer>
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:Skia::skottie>
      $<TARGET_FILE_DIR:skia-consumer>
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE_DIR:Skia::skunicode_icu>/icudtl.dat
      $<TARGET_FILE_DIR:skia-consumer>/icudtl.dat
   VERBATIM
)
