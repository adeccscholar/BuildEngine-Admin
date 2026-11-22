cmake_minimum_required(VERSION 3.31)
project(adecc_NvidiaGDeflateFork LANGUAGES C)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
if(NOT EXISTS "${NVIDIA_LIBDEFLATE_SOURCE}/lib/gdeflate_compress.c")
   message(FATAL_ERROR "Managed NVIDIA GDeflate-capable source is missing")
endif()
set(_root "${NVIDIA_LIBDEFLATE_SOURCE}")
set(_sources
   lib/adler32.c lib/crc32.c lib/deflate_compress.c lib/deflate_decompress.c
   lib/gdeflate_compress.c lib/gdeflate_decompress.c
   lib/gzip_compress.c lib/gzip_decompress.c lib/utils.c
   lib/zlib_compress.c lib/zlib_decompress.c lib/x86/cpu_features.c)
list(TRANSFORM _sources PREPEND "${_root}/")
add_library(libdeflate_gdeflate SHARED ${_sources})
target_compile_definitions(libdeflate_gdeflate PRIVATE LIBDEFLATE_DLL)
target_include_directories(libdeflate_gdeflate PUBLIC
   "$<BUILD_INTERFACE:${_root}>"
   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
set_target_properties(libdeflate_gdeflate PROPERTIES PREFIX "" IMPORT_PREFIX "" WINDOWS_EXPORT_ALL_SYMBOLS ON)
install(TARGETS libdeflate_gdeflate EXPORT LibdeflateGDeflateTargets
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES "${_root}/libdeflate.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
configure_package_config_file(
   "${CMAKE_CURRENT_LIST_DIR}/LibdeflateGDeflateConfig.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/LibdeflateGDeflateConfig.cmake"
   INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/libdeflate-gdeflate/cmake")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/LibdeflateGDeflateConfig.cmake"
   DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/libdeflate-gdeflate/cmake")
install(EXPORT LibdeflateGDeflateTargets NAMESPACE NVIDIA::
   FILE LibdeflateGDeflateTargets.cmake
   DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/libdeflate-gdeflate/cmake")
include(CTest)
if(BUILD_TESTING)
   add_executable(gdeflate-native-api-test
      "${CMAKE_CURRENT_LIST_DIR}/../../smokes/libdeflate-gdeflate/consumer/main.c")
   target_link_libraries(gdeflate-native-api-test PRIVATE libdeflate_gdeflate)
   add_test(NAME gdeflate-native-api COMMAND gdeflate-native-api-test)
endif()
