cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeRaygui LANGUAGES CXX)
if(NOT DEFINED RAYGUI_PACKAGE_ROOT OR NOT DEFINED RAYLIB_PACKAGE_ROOT)
   message(FATAL_ERROR "RAYGUI_PACKAGE_ROOT and RAYLIB_PACKAGE_ROOT are required")
endif()
add_executable(raygui-consumer main.cpp)
target_compile_features(raygui-consumer PRIVATE cxx_std_23)
target_include_directories(raygui-consumer PRIVATE
   "${RAYGUI_PACKAGE_ROOT}/include" "${RAYLIB_PACKAGE_ROOT}/include")
target_link_libraries(raygui-consumer PRIVATE
   "${RAYLIB_PACKAGE_ROOT}/lib/win64/Release/raylib.lib")
