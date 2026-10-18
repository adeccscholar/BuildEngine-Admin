cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeSkia144 LANGUAGES CXX)

find_path(SKIA144_PACKAGE_ROOT
   NAMES include/include/core/SkSurface.h
   PATHS ${CMAKE_PREFIX_PATH}
   NO_DEFAULT_PATH
   REQUIRED)
add_executable(skia144-consumer main.cpp)
set_property(TARGET skia144-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(skia144-consumer PRIVATE cxx_std_23)
target_compile_definitions(skia144-consumer PRIVATE SKIA_DLL WIN32_LEAN_AND_MEAN NOMINMAX)
target_include_directories(skia144-consumer PRIVATE "${SKIA144_PACKAGE_ROOT}/include")
target_link_libraries(skia144-consumer PRIVATE
   "${SKIA144_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/skia144.lib")
add_custom_command(TARGET skia144-consumer POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SKIA144_PACKAGE_ROOT}/bin/win64/${CMAKE_BUILD_TYPE}/skia144.dll"
      "$<TARGET_FILE_DIR:skia144-consumer>"
   VERBATIM)
