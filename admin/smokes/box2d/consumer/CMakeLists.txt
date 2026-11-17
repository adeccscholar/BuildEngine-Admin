cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeBox2D LANGUAGES CXX)
find_package(box2d CONFIG REQUIRED)
add_executable(box2d-consumer main.cpp)
# This consumer uses C++23 without C++ modules. BCC64X does not require
# CMake's separate module dependency scan, which fails before compilation.
set_property(TARGET box2d-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(box2d-consumer PRIVATE cxx_std_23)
target_link_libraries(box2d-consumer PRIVATE box2d::box2d)
