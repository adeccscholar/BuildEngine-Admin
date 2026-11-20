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


# Official upstream platform + renderer backends belong to the managed ImGui
# producer, never to a handwritten per-showcase replacement renderer.
# Keep the core DLL independently usable. The backend has its own DLL/import.
if(IMGUI_BUILD_SDL2_OPENGL3_BACKEND)
   if(NOT DEFINED IMGUI_SDL2_PACKAGE_ROOT OR NOT DEFINED IMGUI_OPENGL_PACKAGE_ROOT OR
      NOT DEFINED IMGUI_GLEW_PACKAGE_ROOT)
      message(FATAL_ERROR "Official SDL2/OpenGL3 backend requires managed SDL2, Mesa and GLEW roots")
   endif()
   set(OpenGL_DIR "${IMGUI_OPENGL_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/cmake/OpenGL")
   set(GLEW_DIR "${IMGUI_GLEW_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/cmake/GLEW")
   find_package(OpenGL CONFIG REQUIRED PATHS "${OpenGL_DIR}" NO_DEFAULT_PATH)
   find_package(GLEW CONFIG REQUIRED PATHS "${GLEW_DIR}" NO_DEFAULT_PATH)
   find_package(SDL2 CONFIG REQUIRED PATHS "${IMGUI_SDL2_PACKAGE_ROOT}" NO_DEFAULT_PATH)
   if(NOT EXISTS "${IMGUI_UPSTREAM_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp" OR
      NOT EXISTS "${IMGUI_UPSTREAM_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
      message(FATAL_ERROR "ImGui 1.93.0 official backend implementation sources are absent")
   endif()
   add_library(imgui_sdl2_opengl3 SHARED
      "${IMGUI_UPSTREAM_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
      "${IMGUI_UPSTREAM_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
   target_compile_features(imgui_sdl2_opengl3 PRIVATE cxx_std_17)
   target_include_directories(imgui_sdl2_opengl3 PRIVATE
      "${IMGUI_UPSTREAM_SOURCE_DIR}"
      "${IMGUI_UPSTREAM_SOURCE_DIR}/backends"
      "${IMGUI_GLEW_PACKAGE_ROOT}/include")
   # Avoid the upstream embedded private OpenGL loader. Reuse BuildEngine GLEW.
   target_compile_definitions(imgui_sdl2_opengl3 PRIVATE
      "IMGUI_API=__declspec(dllimport)"
      "IMGUI_IMPL_API=__declspec(dllexport)"
      IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
   # GLEW supplies the BCC64X-compatible public header as part of its own
   # managed package. No per-consumer forced include is permitted.
   if(NOT EXISTS "${IMGUI_GLEW_PACKAGE_ROOT}/include/GL/glew.h")
      message(FATAL_ERROR "Managed GLEW public header missing")
   endif()
   target_link_libraries(imgui_sdl2_opengl3 PRIVATE
      imgui SDL2::SDL2 OpenGL::OpenGL GLEW::GLEW)
   set_target_properties(imgui_sdl2_opengl3 PROPERTIES
      OUTPUT_NAME "imgui_sdl2_opengl3"
      DEBUG_POSTFIX "d" PREFIX "" IMPORT_PREFIX "lib")
   install(TARGETS imgui_sdl2_opengl3
      RUNTIME DESTINATION "bin/win64/${CMAKE_BUILD_TYPE}"
      LIBRARY DESTINATION "bin/win64/${CMAKE_BUILD_TYPE}"
      ARCHIVE DESTINATION "lib/win64/${CMAKE_BUILD_TYPE}")
endif()
