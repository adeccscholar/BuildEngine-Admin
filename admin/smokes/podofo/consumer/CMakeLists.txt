cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokePoDoFo LANGUAGES CXX)

find_package(PoDoFo 1.1 CONFIG REQUIRED)

add_executable(podofo-consumer main.cpp)
set_property(TARGET podofo-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(podofo-consumer PRIVATE cxx_std_23)
target_link_libraries(podofo-consumer PRIVATE podofo::podofo)
