cmake_minimum_required(VERSION 3.25)
project(BuildEngineDearImGui LANGUAGES CXX)
if(NOT DEFINED IMGUI_UPSTREAM_SOURCE_DIR)
   message(FATAL_ERROR "IMGUI_UPSTREAM_SOURCE_DIR is required")
endif()
add_library(imgui SHARED
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imgui.cpp"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imgui_draw.cpp"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imgui_tables.cpp"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imgui_widgets.cpp"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imgui_demo.cpp")
target_compile_features(imgui PRIVATE cxx_std_17)
target_include_directories(imgui PUBLIC "${IMGUI_UPSTREAM_SOURCE_DIR}")
target_compile_definitions(imgui PRIVATE "IMGUI_API=__declspec(dllexport)")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   set(IMGUI_BINARY_NAME imguid)
else()
   set(IMGUI_BINARY_NAME imgui)
endif()
set_target_properties(imgui PROPERTIES OUTPUT_NAME "${IMGUI_BINARY_NAME}" PREFIX "" IMPORT_PREFIX "lib")
install(TARGETS imgui RUNTIME DESTINATION "bin/win64/${CMAKE_BUILD_TYPE}"
   LIBRARY DESTINATION "bin/win64/${CMAKE_BUILD_TYPE}"
   ARCHIVE DESTINATION "lib/win64/${CMAKE_BUILD_TYPE}")
install(FILES "${IMGUI_UPSTREAM_SOURCE_DIR}/imgui.h"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imconfig.h"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imgui_internal.h"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imstb_rectpack.h"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imstb_textedit.h"
   "${IMGUI_UPSTREAM_SOURCE_DIR}/imstb_truetype.h"
   DESTINATION include)
install(DIRECTORY "${IMGUI_UPSTREAM_SOURCE_DIR}/backends/" DESTINATION include/backends
   FILES_MATCHING PATTERN "*.h")
install(FILES "${IMGUI_UPSTREAM_SOURCE_DIR}/LICENSE.txt" DESTINATION licenses/imgui)
