# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar
cmake_minimum_required(VERSION 3.25)
project(GoogleTestSmoke LANGUAGES CXX)

find_package(GTest CONFIG REQUIRED)

add_executable(GoogleTestSmoke main.cpp)
set_property(TARGET GoogleTestSmoke PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(GoogleTestSmoke PRIVATE cxx_std_17)
target_link_libraries(GoogleTestSmoke PRIVATE GTest::gtest GTest::gmock)
