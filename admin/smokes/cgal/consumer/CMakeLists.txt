cmake_minimum_required(VERSION 3.31)
project(buildengine_cgal_consumer LANGUAGES CXX)

if(NOT DEFINED CGAL_PACKAGE_ROOT OR CGAL_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "CGAL_PACKAGE_ROOT is required")
endif()
if(NOT DEFINED BOOST_PACKAGE_ROOT OR BOOST_PACKAGE_ROOT STREQUAL "")
   message(FATAL_ERROR "BOOST_PACKAGE_ROOT is required")
endif()
if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
   message(FATAL_ERROR "CGAL smoke requires an explicit Release or Debug configuration")
endif()

# The managed CGAL install uses a configuration-specific library directory:
#   lib/win64/<Configuration>/cmake/CGAL/CGALConfig.cmake
# This is not one of CMake's standard directories beneath the install prefix.
# Resolve the actual installed package config, never an incidental system CGAL.
set(CGAL_DIR
   "${CGAL_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/cmake/CGAL"
   CACHE PATH "Managed configuration-specific CGAL CMake package" FORCE)
if(NOT EXISTS "${CGAL_DIR}/CGALConfig.cmake")
   message(FATAL_ERROR
      "Managed CGAL config does not exist: ${CGAL_DIR}/CGALConfig.cmake. "
      "Verify the CGAL install state and CMAKE_BUILD_TYPE.")
endif()

# The BuildEngine integration intentionally uses CGAL's Boost-based number
# types and does not depend on a bundled or external GMP installation.
set(CGAL_DISABLE_GMP ON CACHE BOOL "Managed CGAL excludes GMP" FORCE)
set(CGAL_DO_NOT_USE_BOOST_MP OFF CACHE BOOL "Use managed Boost.Multiprecision" FORCE)
list(PREPEND CMAKE_PREFIX_PATH "${BOOST_PACKAGE_ROOT}")
set(Boost_ROOT "${BOOST_PACKAGE_ROOT}")
set(BOOST_ROOT "${BOOST_PACKAGE_ROOT}")

find_package(CGAL 6.2.1 EXACT CONFIG REQUIRED
   PATHS "${CGAL_DIR}" NO_DEFAULT_PATH)
add_executable(cgal-consumer main.cpp)
target_compile_features(cgal-consumer PRIVATE cxx_std_23)
target_link_libraries(cgal-consumer PRIVATE CGAL::CGAL)
target_include_directories(cgal-consumer PRIVATE "${BOOST_PACKAGE_ROOT}/include")
