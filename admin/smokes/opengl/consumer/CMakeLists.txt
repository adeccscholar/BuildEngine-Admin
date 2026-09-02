cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeopengl LANGUAGES CXX)

find_package(OpenGL CONFIG REQUIRED)

if(NOT EXISTS "${OpenGL_RUNTIME_DLL}")
  message(FATAL_ERROR "Mesa OpenGL runtime not found: ${OpenGL_RUNTIME_DLL}")
endif()
if(NOT EXISTS "${OpenGL_GALLIUM_DLL}")
  message(FATAL_ERROR "Mesa Gallium WGL runtime not found: ${OpenGL_GALLIUM_DLL}")
endif()

add_executable(opengl-consumer main.cpp)
target_compile_features(opengl-consumer PRIVATE cxx_std_23)
target_link_libraries(opengl-consumer PRIVATE OpenGL::OpenGL)

add_custom_command(TARGET opengl-consumer POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${OpenGL_RUNTIME_DLL}" "$<TARGET_FILE_DIR:opengl-consumer>/opengl32.dll"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${OpenGL_GALLIUM_DLL}" "$<TARGET_FILE_DIR:opengl-consumer>/libgallium_wgl.dll"
  VERBATIM)
