cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeOpenMP LANGUAGES CXX)

# Intentionally use BCC64X/LLVM OpenMP as supplied. No downloaded runtime
# and no silent fallback to sequential compilation.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fopenmp" BCC64X_HAS_FOPENMP)
if(NOT BCC64X_HAS_FOPENMP)
   message(FATAL_ERROR "BCC64X OpenMP compiler/link probe failed: -fopenmp is not usable with the available runtime")
endif()

add_executable(openmp-probe main.cpp)
set_property(TARGET openmp-probe PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(openmp-probe PRIVATE cxx_std_23)
target_compile_options(openmp-probe PRIVATE -fopenmp)
target_link_options(openmp-probe PRIVATE -fopenmp)
