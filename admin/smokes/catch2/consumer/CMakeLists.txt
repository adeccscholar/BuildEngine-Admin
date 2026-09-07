# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeCatch2 LANGUAGES CXX)

find_package(Catch2 3.16 CONFIG REQUIRED)

add_executable(catch2-consumer main.cpp)
set_property(TARGET catch2-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(catch2-consumer PRIVATE cxx_std_23)
target_link_libraries(catch2-consumer PRIVATE Catch2::Catch2)
