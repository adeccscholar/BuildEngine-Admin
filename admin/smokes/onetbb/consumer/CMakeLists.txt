cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeOneTBB LANGUAGES CXX)

find_package(TBB 2023.1 CONFIG REQUIRED)

add_executable(onetbb-consumer main.cpp)
set_property(TARGET onetbb-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(onetbb-consumer PRIVATE cxx_std_23)
target_link_libraries(onetbb-consumer PRIVATE TBB::tbb)
