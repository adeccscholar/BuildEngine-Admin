cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeWIL LANGUAGES CXX)

find_package(wil CONFIG REQUIRED)

add_executable(wil-consumer main.cpp)
set_property(TARGET wil-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(wil-consumer PRIVATE cxx_std_23)
target_compile_definitions(wil-consumer PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(wil-consumer PRIVATE WIL::WIL)

if(NOT TARGET WIL::WIL)
   message(FATAL_ERROR "The installed WIL::WIL interface target is missing")
endif()
