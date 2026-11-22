cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeDirectXMath LANGUAGES CXX)
find_package(directxmath CONFIG REQUIRED)
add_executable(directxmath-consumer main.cpp)
set_target_properties(directxmath-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(directxmath-consumer PRIVATE Microsoft::DirectXMath)
