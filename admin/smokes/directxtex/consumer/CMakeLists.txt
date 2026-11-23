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

# Pin the native Release import libraries in the actual BuildEngine package.
# The smoke must not silently succeed against system installations.
set(JPEG_INCLUDE_DIR "${DIRECTXTEX_JPEG_ROOT}/include")
set(PNG_PNG_INCLUDE_DIR "${DIRECTXTEX_PNG_ROOT}/include")
set(ZLIB_INCLUDE_DIR "${DIRECTXTEX_ZLIB_ROOT}/include")
set(ZLIB_LIBRARY "${DIRECTXTEX_ZLIB_ROOT}/lib/win64/Release/libz.lib")
set(PNG_LIBRARY "${DIRECTXTEX_PNG_ROOT}/lib/win64/Release/libpng16.lib")
find_library(JPEG_LIBRARY NAMES jpeg libjpeg
   PATHS "${DIRECTXTEX_JPEG_ROOT}/lib/win64/Release"
   NO_DEFAULT_PATH)
foreach(_required IN ITEMS ZLIB_LIBRARY PNG_LIBRARY JPEG_LIBRARY)
   if(NOT EXISTS "${${_required}}")
      message(FATAL_ERROR "DirectXTex consumer missing import library ${_required}: ${${_required}}")
   endif()
endforeach()
foreach(_required IN ITEMS JPEG_INCLUDE_DIR PNG_PNG_INCLUDE_DIR ZLIB_INCLUDE_DIR)
   if(NOT IS_DIRECTORY "${${_required}}")
      message(FATAL_ERROR "DirectXTex consumer missing header directory ${_required}: ${${_required}}")
   endif()
endforeach()

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

# Resolve the enabled upstream CONFIG dependencies explicitly first.
# Errors now name the actual missing dependency rather than ending solely at
# CMakeFindDependencyMacro.cmake:78.
find_package(Imath CONFIG REQUIRED)
find_package(openjph CONFIG REQUIRED)
find_package(libdeflate CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(OpenEXR CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenMP REQUIRED COMPONENTS CXX)
find_package(directxtex CONFIG REQUIRED)
add_executable(directxtex-consumer main.cpp)
set_target_properties(directxtex-consumer PROPERTIES
   CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(directxtex-consumer PRIVATE Microsoft::DirectXTex)
