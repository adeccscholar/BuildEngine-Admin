cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeOpenXLSX LANGUAGES CXX)

find_package(OpenXLSX CONFIG REQUIRED)

add_executable(openxlsx-consumer main.cpp)
target_compile_features(openxlsx-consumer PRIVATE cxx_std_23)
target_link_libraries(openxlsx-consumer PRIVATE OpenXLSX::OpenXLSX)
