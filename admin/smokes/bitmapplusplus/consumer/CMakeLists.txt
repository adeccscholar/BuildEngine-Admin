# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeBitmapPlusPlus LANGUAGES CXX)

find_package(BitmapPlusPlus 1.1 CONFIG REQUIRED)

add_executable(bitmapplusplus-consumer main.cpp)
set_property(TARGET bitmapplusplus-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(bitmapplusplus-consumer PRIVATE cxx_std_23)
target_link_libraries(bitmapplusplus-consumer PRIVATE bmp::BitmapPlusPlus)
