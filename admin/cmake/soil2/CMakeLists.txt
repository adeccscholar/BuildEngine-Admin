cmake_minimum_required(VERSION 3.25)
project(BuildEngineSOIL2 VERSION 1.3.0 LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT DEFINED SOIL2_SOURCE_DIR OR NOT IS_DIRECTORY "${SOIL2_SOURCE_DIR}")
   message(FATAL_ERROR "SOIL2_SOURCE_DIR must point at the pinned upstream SOIL2 source tree")
endif()

find_package(OpenGL CONFIG REQUIRED)

add_library(SOIL2 SHARED
   "${SOIL2_SOURCE_DIR}/src/SOIL2/image_DXT.c"
   "${SOIL2_SOURCE_DIR}/src/SOIL2/image_helper.c"
   "${SOIL2_SOURCE_DIR}/src/SOIL2/SOIL2.c"
   "${SOIL2_SOURCE_DIR}/src/SOIL2/wfETC.c"
   "${SOIL2_SOURCE_DIR}/src/SOIL2/image_array.c"
)

set_target_properties(SOIL2 PROPERTIES
   OUTPUT_NAME "soil2"
   DEBUG_POSTFIX "d"
   PREFIX ""
   IMPORT_PREFIX ""
   WINDOWS_EXPORT_ALL_SYMBOLS ON
   C_STANDARD 11
   C_STANDARD_REQUIRED ON
)

target_include_directories(SOIL2
   PUBLIC
      $<BUILD_INTERFACE:${SOIL2_SOURCE_DIR}/src/SOIL2>
      $<INSTALL_INTERFACE:include/SOIL2>
)
target_link_libraries(SOIL2 PRIVATE OpenGL::OpenGL)

install(TARGETS SOIL2
   EXPORT SOIL2Targets
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(FILES
   "${SOIL2_SOURCE_DIR}/src/SOIL2/SOIL2.h"
   "${SOIL2_SOURCE_DIR}/src/SOIL2/image_array.h"
   "${SOIL2_SOURCE_DIR}/src/SOIL2/image_array_helper.h"
   DESTINATION include/SOIL2
)

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/SOIL2Config.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/SOIL2Config.cmake"
   INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SOIL2"
)
write_basic_package_version_file(
   "${CMAKE_CURRENT_BINARY_DIR}/SOIL2ConfigVersion.cmake"
   VERSION "1.3.0"
   COMPATIBILITY SameMajorVersion
)
install(FILES
   "${CMAKE_CURRENT_BINARY_DIR}/SOIL2Config.cmake"
   "${CMAKE_CURRENT_BINARY_DIR}/SOIL2ConfigVersion.cmake"
   DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SOIL2"
)
install(EXPORT SOIL2Targets
   NAMESPACE SOIL2::
   DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SOIL2"
)
