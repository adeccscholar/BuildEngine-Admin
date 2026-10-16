cmake_minimum_required(VERSION 3.25)
project(agg VERSION 2.6 LANGUAGES CXX)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Only upstream AGG core sources. GPC and platform/demo sources are excluded.
file(GLOB AGG_CORE_SOURCES CONFIGURE_DEPENDS
   "${CMAKE_CURRENT_SOURCE_DIR}/agg-src/src/agg_*.cpp")
if(NOT AGG_CORE_SOURCES)
   message(FATAL_ERROR "AGG core sources not found")
endif()
add_library(agg STATIC ${AGG_CORE_SOURCES})
add_library(agg::agg ALIAS agg)
target_compile_features(agg PUBLIC cxx_std_17)
target_include_directories(agg PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/agg-src/include>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
set_target_properties(agg PROPERTIES EXPORT_NAME agg)

install(TARGETS agg EXPORT aggTargets
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/agg-src/include/"
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   FILES_MATCHING PATTERN "agg_*.h")
# Do not install or compile agg-src/gpc or any GPC examples.
install(EXPORT aggTargets FILE aggTargets.cmake NAMESPACE agg::
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/agg)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/agg-config.cmake"
   "include(\"\${CMAKE_CURRENT_LIST_DIR}/aggTargets.cmake\")\n")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/agg-config.cmake"
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/agg)
