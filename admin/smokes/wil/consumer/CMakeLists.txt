cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeWIL LANGUAGES CXX)

find_package(wil CONFIG REQUIRED)

add_executable(wil-consumer main.cpp)
set_property(TARGET wil-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(wil-consumer PRIVATE cxx_std_23)
target_compile_definitions(wil-consumer PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(wil-consumer PRIVATE WIL::WIL)

# WeakReference.h is supplied in the Windows SDK winrt include directory,
# not by WIL. Resolve it relative to BuildEngine's registered RC executable.
if(NOT DEFINED ENV{CB_WINDOWS_RC} OR "$ENV{CB_WINDOWS_RC}" STREQUAL "")
   message(FATAL_ERROR "CB_WINDOWS_RC is required to resolve the registered Windows SDK")
endif()
get_filename_component(_sdk_bin "$ENV{CB_WINDOWS_RC}" DIRECTORY)
get_filename_component(_sdk_arch "${_sdk_bin}" NAME)
get_filename_component(_sdk_version_dir "${_sdk_bin}" DIRECTORY)
get_filename_component(_sdk_version "${_sdk_version_dir}" NAME)
get_filename_component(_sdk_bin_root "${_sdk_version_dir}" DIRECTORY)
get_filename_component(_sdk_root "${_sdk_bin_root}" DIRECTORY)
set(_sdk_winrt "${_sdk_root}/Include/${_sdk_version}/winrt")
if(NOT EXISTS "${_sdk_winrt}/WeakReference.h")
   message(FATAL_ERROR "Registered Windows SDK is missing winrt/WeakReference.h: ${_sdk_winrt}")
endif()
target_include_directories(wil-consumer SYSTEM PRIVATE "${_sdk_winrt}")

if(NOT TARGET WIL::WIL)
   message(FATAL_ERROR "The installed WIL::WIL interface target is missing")
endif()
