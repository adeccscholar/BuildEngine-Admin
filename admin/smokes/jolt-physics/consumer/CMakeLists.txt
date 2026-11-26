cmake_minimum_required(VERSION 3.31)
project(buildengine_jolt_consumer LANGUAGES CXX)
if(NOT DEFINED JOLT_PACKAGE_ROOT)
   message(FATAL_ERROR "JOLT_PACKAGE_ROOT is required")
endif()
add_executable(jolt-consumer-real main.cpp)
target_compile_features(jolt-consumer-real PRIVATE cxx_std_23)
add_executable(jolt-consumer loader.cpp)
target_compile_features(jolt-consumer PRIVATE cxx_std_23)
# Match the public compile definitions of the installed Jolt 5.7.0 Release DLL.
# JPH_VERSION_ID includes object layer width, object stream, renderer and
# profiler features. These must match on both sides of the shared boundary.
target_compile_definitions(jolt-consumer-real PRIVATE
   JPH_SHARED_LIBRARY
   JPH_OBJECT_LAYER_BITS=16
   JPH_OBJECT_STREAM
   JPH_DEBUG_RENDERER
   JPH_PROFILE_ENABLED
   # Baseline x64 SSE2; no optional ISA feature defines.)
target_include_directories(jolt-consumer-real PRIVATE "${JOLT_PACKAGE_ROOT}/include")
find_library(JOLT_LIBRARY NAMES Jolt libJolt PATHS "${JOLT_PACKAGE_ROOT}/lib/win64/Release" NO_DEFAULT_PATH REQUIRED)
target_link_libraries(jolt-consumer-real PRIVATE "${JOLT_LIBRARY}")

# The smoke is an application deployment, not a PATH-based probe.
# Stage the declared package closure and the exact BCC64X C++ runtime
# beside the executable, as in the other BuildEngine consumers.
include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/consumer/BuildEngineRuntime.cmake")
if(DEFINED ADECC_RUNTIME_PACKAGE_ROOTS AND
   NOT "${ADECC_RUNTIME_PACKAGE_ROOTS}" STREQUAL "")
   set(vecJoltRuntimeRoots ${ADECC_RUNTIME_PACKAGE_ROOTS})
else()
   set(vecJoltRuntimeRoots "${JOLT_PACKAGE_ROOT}")
endif()
adecc_stage_runtime(jolt-consumer
   PACKAGE_ROOTS ${vecJoltRuntimeRoots}
   TOOLCHAIN_DLLS libc++-370.dll)
