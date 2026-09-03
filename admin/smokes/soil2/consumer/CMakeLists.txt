cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeSOIL2 LANGUAGES CXX)

find_package(SOIL2 1.3 CONFIG REQUIRED)

add_executable(soil2-consumer main.cpp)
set_property(TARGET soil2-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(soil2-consumer PRIVATE cxx_std_23)
target_link_libraries(soil2-consumer PRIVATE SOIL2::SOIL2)
