cmake_minimum_required(VERSION 3.31)
project(buildengine_cgal_consumer LANGUAGES CXX)

if(NOT DEFINED CGAL_PACKAGE_ROOT OR CGAL_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "CGAL_PACKAGE_ROOT is required")
endif()
if(NOT DEFINED BOOST_PACKAGE_ROOT OR BOOST_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "BOOST_PACKAGE_ROOT is required")
endif()
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
   message(FATAL_ERROR "CGAL smoke requires explicit Release or Debug")
endif()

set(CGAL_DIR
   "${CGAL_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/cmake/CGAL"
   CACHE PATH "Managed configuration-specific CGAL CMake package" FORCE)
foreach(theFile IN ITEMS
   "${CGAL_DIR}/CGALConfig.cmake"
   "${CGAL_DIR}/CGALConfig-installation-dirs.cmake"
   "${CGAL_PACKAGE_ROOT}/include/CGAL/config.h"
   "${CGAL_PACKAGE_ROOT}/include/CGAL/Simple_cartesian.h")
   if(NOT EXISTS "${theFile}")
      message(FATAL_ERROR "CGAL installed package artifact missing: ${theFile}")
   endif()
endforeach()
message(STATUS "[CGAL] Checked installed package root: ${CGAL_PACKAGE_ROOT}")
message(STATUS "[CGAL] Checked CMake package: ${CGAL_DIR}")
# Pre-resolve the exact install root even when the package config does not
# include its optional companion during find_package(). This keeps the
# standard CGAL package and target checks intact.
include("${CGAL_DIR}/CGALConfig-installation-dirs.cmake")

set(CGAL_DISABLE_GMP ON CACHE BOOL "CGAL uses the managed Boost number types" FORCE)
set(CGAL_DO_NOT_USE_BOOST_MP OFF CACHE BOOL "Use managed Boost.Multiprecision" FORCE)
list(PREPEND CMAKE_PREFIX_PATH "${BOOST_PACKAGE_ROOT}")
set(Boost_ROOT "${BOOST_PACKAGE_ROOT}")
set(BOOST_ROOT "${BOOST_PACKAGE_ROOT}")

find_package(CGAL 6.2.1 EXACT CONFIG REQUIRED
   PATHS "${CGAL_DIR}" NO_DEFAULT_PATH)
if(NOT TARGET CGAL::CGAL)
   message(FATAL_ERROR "Installed CGAL config did not define CGAL::CGAL")
endif()
add_executable(cgal-consumer main.cpp)
target_compile_features(cgal-consumer PRIVATE cxx_std_23)
target_link_libraries(cgal-consumer PRIVATE CGAL::CGAL)
target_include_directories(cgal-consumer PRIVATE "${BOOST_PACKAGE_ROOT}/include")

# CGAL is header-only, but its independent consumer still needs the exact
# managed BCC64X runtime closure if imported Boost dependencies require DLLs.
include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/consumer/BuildEngineRuntime.cmake")
if(NOT DEFINED ADECC_RUNTIME_PACKAGE_ROOTS OR
   "${ADECC_RUNTIME_PACKAGE_ROOTS}" STREQUAL "")
   message(FATAL_ERROR "Missing managed CGAL smoke runtime dependency closure")
endif()
adecc_stage_runtime(cgal-consumer
   PACKAGE_ROOTS ${ADECC_RUNTIME_PACKAGE_ROOTS})
