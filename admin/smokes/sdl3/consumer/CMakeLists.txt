cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeSDL3 LANGUAGES CXX)
find_package(SDL3 CONFIG REQUIRED)
add_executable(sdl3-consumer main.cpp)
# This standalone C++23 smoke does not use modules. Disable the optional
# CMake module dependency scan, which fails on BCC64X before compilation.
set_property(TARGET sdl3-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(sdl3-consumer PRIVATE cxx_std_23)
target_link_libraries(sdl3-consumer PRIVATE SDL3::SDL3)
