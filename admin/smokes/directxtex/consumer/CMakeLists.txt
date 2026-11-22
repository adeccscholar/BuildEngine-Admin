cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeDirectXTex LANGUAGES CXX)
find_package(directxtex CONFIG REQUIRED)
add_executable(directxtex-consumer main.cpp)
set_target_properties(directxtex-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(directxtex-consumer PRIVATE Microsoft::DirectXTex)
