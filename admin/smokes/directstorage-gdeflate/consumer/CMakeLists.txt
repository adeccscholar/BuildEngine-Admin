cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeDirectStorageGDeflate LANGUAGES CXX)
find_package(GDeflate CONFIG REQUIRED)
add_executable(directstorage-gdeflate-consumer main.cpp)
set_target_properties(directstorage-gdeflate-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(directstorage-gdeflate-consumer PRIVATE Microsoft::GDeflate)
