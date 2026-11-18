cmake_minimum_required(VERSION 3.25)
project(BuildEngineImPlot LANGUAGES CXX)
if(NOT DEFINED IMPLOT_UPSTREAM_SOURCE_DIR OR NOT DEFINED IMGUI_PACKAGE_ROOT)
   message(FATAL_ERROR "IMPLOT_UPSTREAM_SOURCE_DIR and IMGUI_PACKAGE_ROOT are required")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   set(IMGUI_BINARY_NAME imguid)
   set(IMPLOT_BINARY_NAME implotd)
else()
   set(IMGUI_BINARY_NAME imgui)
   set(IMPLOT_BINARY_NAME implot)
endif()
set(IMGUI_IMPORT "${IMGUI_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/lib${IMGUI_BINARY_NAME}.lib")
if(NOT EXISTS "${IMGUI_IMPORT}")
   message(FATAL_ERROR "Missing Dear ImGui import library: ${IMGUI_IMPORT}")
endif()
add_library(implot SHARED
   "${IMPLOT_UPSTREAM_SOURCE_DIR}/implot.cpp"
   "${IMPLOT_UPSTREAM_SOURCE_DIR}/implot_items.cpp"
   "${IMPLOT_UPSTREAM_SOURCE_DIR}/implot_demo.cpp")
target_compile_features(implot PRIVATE cxx_std_17)
target_include_directories(implot PUBLIC
   "${IMPLOT_UPSTREAM_SOURCE_DIR}" "${IMGUI_PACKAGE_ROOT}/include")
target_compile_definitions(implot PRIVATE
   "IMPLOT_API=__declspec(dllexport)" "IMGUI_API=__declspec(dllimport)")
target_link_libraries(implot PRIVATE "${IMGUI_IMPORT}")
set_target_properties(implot PROPERTIES OUTPUT_NAME "${IMPLOT_BINARY_NAME}" PREFIX "" IMPORT_PREFIX "lib")
install(TARGETS implot RUNTIME DESTINATION "bin/win64/${CMAKE_BUILD_TYPE}"
   LIBRARY DESTINATION "bin/win64/${CMAKE_BUILD_TYPE}"
   ARCHIVE DESTINATION "lib/win64/${CMAKE_BUILD_TYPE}")
install(FILES "${IMPLOT_UPSTREAM_SOURCE_DIR}/implot.h"
   "${IMPLOT_UPSTREAM_SOURCE_DIR}/implot_internal.h" DESTINATION include)
install(FILES "${IMPLOT_UPSTREAM_SOURCE_DIR}/LICENSE" DESTINATION licenses/implot)
