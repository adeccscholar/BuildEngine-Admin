cmake_minimum_required(VERSION 3.31)
project(adecc_BCC64X_Detours LANGUAGES CXX)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
if(NOT DEFINED DETOURS_UPSTREAM_SOURCE OR NOT EXISTS "${DETOURS_UPSTREAM_SOURCE}/src/detours.h")
   message(FATAL_ERROR "Managed Detours 4.0.1 source missing")
endif()
set(_src "${DETOURS_UPSTREAM_SOURCE}/src")
add_library(detours SHARED
   "${_src}/detours.cpp" "${_src}/modules.cpp" "${_src}/disasm.cpp"
   "${_src}/image.cpp" "${_src}/creatwth.cpp" "${_src}/disolx86.cpp"
   "${_src}/disolx64.cpp" "${_src}/disolia64.cpp" "${_src}/disolarm.cpp"
   "${_src}/disolarm64.cpp")
target_include_directories(detours PUBLIC
   "$<BUILD_INTERFACE:${_src}>"
   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_compile_definitions(detours PRIVATE WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0601)
target_link_libraries(detours PRIVATE kernel32 advapi32)
set_target_properties(detours PROPERTIES OUTPUT_NAME detours PREFIX "" IMPORT_PREFIX ""
   WINDOWS_EXPORT_ALL_SYMBOLS ON EXPORT_NAME Detours)
install(TARGETS detours EXPORT DetoursTargets
   RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
   LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
   ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES "${_src}/detours.h" "${_src}/detver.h"
   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
configure_package_config_file(
   "${CMAKE_CURRENT_LIST_DIR}/DetoursConfig.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/DetoursConfig.cmake"
   INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/detours/cmake")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/DetoursConfig.cmake"
   DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/detours/cmake")
install(EXPORT DetoursTargets FILE DetoursTargets.cmake NAMESPACE Microsoft::
   DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/detours/cmake")
include(CTest)
if(BUILD_TESTING)
   if(NOT DEFINED DETOURS_TEST_SOURCE OR NOT EXISTS "${DETOURS_TEST_SOURCE}")
      message(FATAL_ERROR "Detours transaction test source must be supplied")
   endif()
   add_executable(detours-native-test "${DETOURS_TEST_SOURCE}")
   target_compile_features(detours-native-test PRIVATE cxx_std_23)
   target_link_libraries(detours-native-test PRIVATE detours)
   add_test(NAME detours-transaction COMMAND detours-native-test)
endif()
