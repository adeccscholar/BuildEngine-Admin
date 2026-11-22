cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeDetours LANGUAGES CXX)
find_package(Detours CONFIG REQUIRED)
add_executable(detours-consumer main.cpp)
set_target_properties(detours-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(detours-consumer PRIVATE Microsoft::Detours)
