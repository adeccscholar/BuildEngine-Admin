cmake_minimum_required(VERSION 3.31)
project(adecc_DirectStorage_GDeflate LANGUAGES CXX)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
if(NOT EXISTS "${DEFLATE_SOURCE_ROOT}/GDeflate.h")
   message(FATAL_ERROR "Managed DirectStorage/GDeflate source missing")
endif()
if(NOT EXISTS "${LIBDEFLATE_PACKAGE_ROOT}/include/libdeflate.h")
   message(FATAL_ERROR "Managed libdeflate package missing")
endif()
find_package(LibdeflateGDeflate CONFIG REQUIRED
   PATHS "${LIBDEFLATE_PACKAGE_ROOT}/share/libdeflate-gdeflate/cmake" NO_DEFAULT_PATH)
add_library(GDeflate SHARED
   "${DEFLATE_SOURCE_ROOT}/GDeflateCompress.cpp"
   "${DEFLATE_SOURCE_ROOT}/GDeflateDecompress.cpp")
target_include_directories(GDeflate PUBLIC
   "$<BUILD_INTERFACE:${DEFLATE_SOURCE_ROOT}>"
   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_include_directories(GDeflate PRIVATE "${LIBDEFLATE_PACKAGE_ROOT}/include")
target_link_libraries(GDeflate PRIVATE NVIDIA::libdeflate_gdeflate runtimeobject)
set_target_properties(GDeflate PROPERTIES PREFIX "" IMPORT_PREFIX ""
   WINDOWS_EXPORT_ALL_SYMBOLS ON)
install(TARGETS GDeflate EXPORT GDeflateTargets
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES "${DEFLATE_SOURCE_ROOT}/GDeflate.h" "${DEFLATE_SOURCE_ROOT}/config.h"
   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
# Retain the Microsoft GDeflate license at the originally declared SBOM path.
# BuildEngine also copies it into licenses/gdeflate/LICENSE during common install.
if(NOT EXISTS "${DEFLATE_SOURCE_ROOT}/LICENSE")
   message(FATAL_ERROR "Microsoft GDeflate source license missing: ${DEFLATE_SOURCE_ROOT}/LICENSE")
endif()
install(FILES "${DEFLATE_SOURCE_ROOT}/LICENSE" DESTINATION "GDeflate/GDeflate")
configure_package_config_file(
   "${CMAKE_CURRENT_LIST_DIR}/GDeflateConfig.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/GDeflateConfig.cmake"
   INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/gdeflate/cmake")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/GDeflateConfig.cmake"
   DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/gdeflate/cmake")
install(EXPORT GDeflateTargets FILE GDeflateTargets.cmake NAMESPACE Microsoft::
   DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/gdeflate/cmake")
include(CTest)
if(BUILD_TESTING)
   if(NOT EXISTS "${GDEFLATE_NATIVE_TEST}")
      message(FATAL_ERROR "Roundtrip native test missing")
   endif()
   add_executable(gdeflate-roundtrip "${GDEFLATE_NATIVE_TEST}")
   target_link_libraries(gdeflate-roundtrip PRIVATE GDeflate)
   add_test(NAME gdeflate-roundtrip COMMAND gdeflate-roundtrip)
endif()
