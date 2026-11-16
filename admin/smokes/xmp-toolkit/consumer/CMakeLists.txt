cmake_minimum_required(VERSION 3.31)
project(xmp_toolkit_consumer LANGUAGES CXX)
if(NOT DEFINED ENV{XMP_TOOLKIT_ROOT} OR NOT IS_DIRECTORY "$ENV{XMP_TOOLKIT_ROOT}")
   message(FATAL_ERROR "XMP_TOOLKIT_ROOT must point to the installed package")
endif()
set(theRoot "$ENV{XMP_TOOLKIT_ROOT}")
add_executable(xmp-toolkit-consumer main.cpp)
set_target_properties(xmp-toolkit-consumer PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
set_property(TARGET xmp-toolkit-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_include_directories(xmp-toolkit-consumer PRIVATE "${theRoot}/include/xmp")
target_link_libraries(xmp-toolkit-consumer PRIVATE
   "${theRoot}/lib/win64/Release/XMPCore.lib"
   "${theRoot}/lib/win64/Release/XMPFiles.lib")
