cmake_minimum_required(VERSION 3.31)
project(buildengine_libigl_consumer LANGUAGES CXX)
if(NOT DEFINED LIBIGL_PACKAGE_ROOT OR NOT DEFINED EIGEN_PACKAGE_ROOT)
   message(FATAL_ERROR "LIBIGL_PACKAGE_ROOT and EIGEN_PACKAGE_ROOT are required")
endif()
add_executable(libigl-consumer main.cpp)
target_compile_features(libigl-consumer PRIVATE cxx_std_23)
target_include_directories(libigl-consumer PRIVATE
   "${LIBIGL_PACKAGE_ROOT}/include"
   "${EIGEN_PACKAGE_ROOT}/include/eigen3")
