cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeDirectXMesh LANGUAGES CXX)
find_package(directxmesh CONFIG REQUIRED)
add_executable(directxmesh-consumer main.cpp)
set_target_properties(directxmesh-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(directxmesh-consumer PRIVATE Microsoft::DirectXMesh)
