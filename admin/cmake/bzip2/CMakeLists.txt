cmake_minimum_required(VERSION 3.25)
project(AdeccBZip2 VERSION 1.0.8 LANGUAGES C)
include(GNUInstallDirs)
if(NOT DEFINED BZIP2_SOURCE_DIR)
  message(FATAL_ERROR "BZIP2_SOURCE_DIR is required")
endif()

# bzip2 1.0.8 ships a legacy Windows module-definition file whose LIBRARY
# name does not match the managed package name and whose DESCRIPTION directive
# is rejected by BCC64X/LLD. Keep the upstream source unchanged and preserve
# its explicit public export list in a generated linker-compatible DEF file.
set(BZIP2_DEF_FILE "${CMAKE_CURRENT_BINARY_DIR}/libbz2-bcc64x.def")
file(STRINGS "${BZIP2_SOURCE_DIR}/libbz2.def" BZIP2_DEF_LINES)
list(FILTER BZIP2_DEF_LINES EXCLUDE REGEX "^[ \t]*(LIBRARY|DESCRIPTION)([ \t]|$)")
list(JOIN BZIP2_DEF_LINES "\n" BZIP2_DEF_CONTENT)
file(WRITE "${BZIP2_DEF_FILE}" "${BZIP2_DEF_CONTENT}\n")

add_library(bzip2 SHARED
  "${BZIP2_SOURCE_DIR}/blocksort.c" "${BZIP2_SOURCE_DIR}/huffman.c" "${BZIP2_SOURCE_DIR}/crctable.c"
  "${BZIP2_SOURCE_DIR}/randtable.c" "${BZIP2_SOURCE_DIR}/compress.c" "${BZIP2_SOURCE_DIR}/decompress.c"
  "${BZIP2_SOURCE_DIR}/bzlib.c" "${BZIP2_DEF_FILE}")
target_include_directories(bzip2 PUBLIC $<BUILD_INTERFACE:${BZIP2_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
set_target_properties(bzip2 PROPERTIES OUTPUT_NAME bzip2 DEBUG_POSTFIX d PREFIX "" IMPORT_PREFIX "")
install(TARGETS bzip2 EXPORT BZip2Targets RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(EXPORT BZip2Targets FILE BZip2Targets.cmake NAMESPACE BZip2:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/BZip2")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/BZip2Config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/BZip2Config.cmake" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/BZip2Config.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/BZip2")
