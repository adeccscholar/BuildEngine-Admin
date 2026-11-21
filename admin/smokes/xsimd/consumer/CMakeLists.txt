cmake_minimum_required(VERSION 3.31)
project(buildengine_xsimd_consumer LANGUAGES CXX)
if(NOT DEFINED XSIMD_PACKAGE_ROOT)
   message(FATAL_ERROR "XSIMD_PACKAGE_ROOT is required")
endif()
add_executable(xsimd-consumer main.cpp)
target_compile_features(xsimd-consumer PRIVATE cxx_std_23)
target_include_directories(xsimd-consumer PRIVATE
   "${XSIMD_PACKAGE_ROOT}/include")
