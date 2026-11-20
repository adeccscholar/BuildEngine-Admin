cmake_minimum_required(VERSION 3.31)
project(buildengine_jolt_consumer LANGUAGES CXX)
if(NOT DEFINED JOLT_PACKAGE_ROOT)
   message(FATAL_ERROR "JOLT_PACKAGE_ROOT is required")
endif()
add_executable(jolt-consumer main.cpp)
target_compile_features(jolt-consumer PRIVATE cxx_std_23)
# Match the public compile definitions of the installed Jolt 5.7.0 Release DLL.
# JPH_VERSION_ID includes object layer width, object stream, renderer and
# profiler features. These must match on both sides of the shared boundary.
target_compile_definitions(jolt-consumer PRIVATE
   JPH_SHARED_LIBRARY
   JPH_OBJECT_LAYER_BITS=16
   JPH_OBJECT_STREAM
   JPH_DEBUG_RENDERER
   JPH_PROFILE_ENABLED
   JPH_USE_SSE4_1
   JPH_USE_SSE4_2
   JPH_USE_AVX
   JPH_USE_AVX2
   JPH_USE_LZCNT
   JPH_USE_TZCNT
   JPH_USE_F16C
   JPH_USE_FMADD)
target_include_directories(jolt-consumer PRIVATE "${JOLT_PACKAGE_ROOT}/include")
find_library(JOLT_LIBRARY NAMES Jolt libJolt PATHS "${JOLT_PACKAGE_ROOT}/lib/win64/Release" NO_DEFAULT_PATH REQUIRED)
target_link_libraries(jolt-consumer PRIVATE "${JOLT_LIBRARY}")
