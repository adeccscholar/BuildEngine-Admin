cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeDirectXHeaders LANGUAGES CXX)
find_package(directx-headers 1.619.1 EXACT CONFIG REQUIRED)
add_executable(directx-headers-consumer main.cpp)
set_target_properties(directx-headers-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(directx-headers-consumer PRIVATE
   Microsoft::DirectX-Headers Microsoft::DirectX-Guids)
