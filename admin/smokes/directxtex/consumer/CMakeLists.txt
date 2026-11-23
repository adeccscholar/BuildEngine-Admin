cmake_minimum_required(VERSION 3.31)
project(BuildEngineSmokeDirectXTex LANGUAGES CXX)

# The installed DirectXTex CONFIG package exports its transitive codec,
# OpenEXR and OpenMP requirements. Resolve exactly the packages produced by
# BuildEngine rather than relying on globally installed SDKs.
foreach(strRequired IN ITEMS DIRECTXTEX_OPENEXR_ROOT DIRECTXTEX_IMATH_ROOT
      DIRECTXTEX_OPENJPH_ROOT DIRECTXTEX_LIBDEFLATE_ROOT DIRECTXTEX_ZSTD_ROOT
      DIRECTXTEX_JPEG_ROOT DIRECTXTEX_PNG_ROOT DIRECTXTEX_ZLIB_ROOT
      DIRECTXTEX_OPENMP_ROOT)
   if(NOT DEFINED ${strRequired} OR NOT IS_DIRECTORY "${${strRequired}}")
      message(FATAL_ERROR "Missing BuildEngine dependency root: ${strRequired}")
   endif()
endforeach()

list(PREPEND CMAKE_PREFIX_PATH
   "${DIRECTXTEX_OPENEXR_ROOT}" "${DIRECTXTEX_IMATH_ROOT}"
   "${DIRECTXTEX_OPENJPH_ROOT}" "${DIRECTXTEX_LIBDEFLATE_ROOT}"
   "${DIRECTXTEX_ZSTD_ROOT}" "${DIRECTXTEX_JPEG_ROOT}"
   "${DIRECTXTEX_PNG_ROOT}" "${DIRECTXTEX_ZLIB_ROOT}"
   "${DIRECTXTEX_OPENMP_ROOT}")
set(OpenEXR_DIR "${DIRECTXTEX_OPENEXR_ROOT}/lib/win64/Release/cmake/OpenEXR")
set(Imath_DIR "${DIRECTXTEX_IMATH_ROOT}/lib/win64/Release/cmake/Imath")
set(openjph_DIR "${DIRECTXTEX_OPENJPH_ROOT}/lib/win64/Release/cmake/openjph")
set(libdeflate_DIR "${DIRECTXTEX_LIBDEFLATE_ROOT}/lib/win64/Release/cmake/libdeflate")
set(zstd_DIR "${DIRECTXTEX_ZSTD_ROOT}/lib/win64/Release/cmake/zstd")
set(JPEG_ROOT "${DIRECTXTEX_JPEG_ROOT}")
set(PNG_ROOT "${DIRECTXTEX_PNG_ROOT}")
set(ZLIB_ROOT "${DIRECTXTEX_ZLIB_ROOT}")

# The BCC64X toolchain is Clang/EMBT; CMake's generic OpenMP probe is not
# sufficient to locate the separately built LLVM OpenMP runtime. Supply the
# actual dependency artifact and let the upstream CONFIG package run its
# original find_dependency(OpenMP) without disabling OpenMP.
set(OpenMP_CXX_FLAGS "-fopenmp")
set(OpenMP_CXX_LIB_NAMES "omp")
set(OpenMP_omp_LIBRARY "${DIRECTXTEX_OPENMP_ROOT}/lib/win64/Release/libomp.lib")
if(NOT EXISTS "${OpenMP_omp_LIBRARY}")
   message(FATAL_ERROR "BuildEngine LLVM OpenMP import library missing: ${OpenMP_omp_LIBRARY}")
endif()

find_package(directxtex CONFIG REQUIRED)
add_executable(directxtex-consumer main.cpp)
set_target_properties(directxtex-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(directxtex-consumer PRIVATE Microsoft::DirectXTex)
