cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeOneTBBTerminate LANGUAGES CXX)

find_package(TBB 2023.1 CONFIG REQUIRED)

add_executable(onetbb-terminate-smoke terminate_smoke.cpp)
set_property(TARGET onetbb-terminate-smoke PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(onetbb-terminate-smoke PRIVATE cxx_std_23)
target_link_libraries(onetbb-terminate-smoke PRIVATE TBB::tbb)
