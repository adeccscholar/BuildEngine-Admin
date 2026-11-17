cmake_minimum_required(VERSION 3.31)
project(BuildEngineSDL3 LANGUAGES C CXX)

if(NOT DEFINED SDL3_UPSTREAM_SOURCE_DIR OR NOT EXISTS "${SDL3_UPSTREAM_SOURCE_DIR}/CMakeLists.txt")
   message(FATAL_ERROR "SDL3_UPSTREAM_SOURCE_DIR must contain the pinned SDL3 sources")
endif()

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL ON CACHE BOOL "" FORCE)
set(SDL_INSTALL_DOCS OFF CACHE BOOL "" FORCE)

# BCC64X uses the registered Windows SDK; it is neither MSVC nor MinGW.
# SDL3's upstream DirectX probe otherwise wrongly requires the obsolete DXSDK_DIR.
# Keep native Windows DirectX support enabled and use the SDK headers and libs.
set(USE_WINSDK_DIRECTX TRUE)

add_subdirectory("${SDL3_UPSTREAM_SOURCE_DIR}" upstream)

if(NOT TARGET SDL3-shared)
   message(FATAL_ERROR "Pinned SDL3 upstream DLL target is missing")
endif()
# SDL3 enables PCH for both C and C++ at its upstream target level.
# BCC64X rejects the generated C PCH when translation units use a different
# effective C language mode (E904: C11 enabled in the AST, disabled in TU).
# Disable PCH for this package only, preserving all SDL3 sources and features.
set_target_properties(SDL3-shared PROPERTIES
   DISABLE_PRECOMPILE_HEADERS TRUE
   PREFIX ""
   IMPORT_PREFIX ""
   OUTPUT_NAME "SDL3"
   DEBUG_POSTFIX "d"
)
