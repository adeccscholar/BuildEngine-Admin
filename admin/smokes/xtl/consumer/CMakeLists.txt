cmake_minimum_required(VERSION 3.31)
project(buildengine_xtl_consumer LANGUAGES CXX)
if(NOT DEFINED XTL_PACKAGE_ROOT)
   message(FATAL_ERROR "XTL_PACKAGE_ROOT is required")
endif()
add_executable(xtl-consumer main.cpp)
target_compile_features(xtl-consumer PRIVATE cxx_std_23)
target_include_directories(xtl-consumer PRIVATE
   "${XTL_PACKAGE_ROOT}/include")
