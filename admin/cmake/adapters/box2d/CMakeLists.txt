cmake_minimum_required(VERSION 3.31)
project(BuildEngineBox2D LANGUAGES C CXX)

if(NOT DEFINED BOX2D_UPSTREAM_SOURCE_DIR OR NOT EXISTS "${BOX2D_UPSTREAM_SOURCE_DIR}/CMakeLists.txt")
   message(FATAL_ERROR "BOX2D_UPSTREAM_SOURCE_DIR must contain the pinned Box2D sources")
endif()

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
set(BOX2D_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(BOX2D_DOCS OFF CACHE BOOL "" FORCE)
set(BOX2D_PROFILE OFF CACHE BOOL "" FORCE)
set(BOX2D_COMPILE_WARNING_AS_ERROR OFF CACHE BOOL "" FORCE)
set(BOX2D_DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
set(BOX2D_VALIDATE ON CACHE BOOL "" FORCE)

add_subdirectory("${BOX2D_UPSTREAM_SOURCE_DIR}" upstream)

if(NOT TARGET box2d)
   message(FATAL_ERROR "Pinned Box2D upstream DLL target is missing")
endif()
set_target_properties(box2d PROPERTIES PREFIX "" OUTPUT_NAME "box2d" DEBUG_POSTFIX "d")
# Upstream has a native Windows DLL export contract (box2d_EXPORTS).
# Propagate its public DLL import define for every BCC64X consumer.
target_compile_definitions(box2d INTERFACE BOX2D_DLL)
