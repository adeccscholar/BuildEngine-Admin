cmake_minimum_required(VERSION 3.31)
project(buildengine_xtensor_consumer LANGUAGES CXX)
if(NOT DEFINED XTENSOR_PACKAGE_ROOT)
   message(FATAL_ERROR "XTENSOR_PACKAGE_ROOT is required")
endif()
if(NOT DEFINED XTL_PACKAGE_ROOT)
   message(FATAL_ERROR "XTL_PACKAGE_ROOT is required")
endif()
if(NOT DEFINED XSIMD_PACKAGE_ROOT)
   message(FATAL_ERROR "XSIMD_PACKAGE_ROOT is required")
endif()
add_executable(xtensor-consumer main.cpp)
target_compile_features(xtensor-consumer PRIVATE cxx_std_23)
target_include_directories(xtensor-consumer PRIVATE
   "${XTENSOR_PACKAGE_ROOT}/include"
   "${XTL_PACKAGE_ROOT}/include"
   "${XSIMD_PACKAGE_ROOT}/include")
target_compile_definitions(xtensor-consumer PRIVATE XTENSOR_USE_XSIMD)
