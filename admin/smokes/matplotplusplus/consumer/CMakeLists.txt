cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeMatplot LANGUAGES CXX)

if(NOT DEFINED MATPLOT_PACKAGE_ROOT)
   message(FATAL_ERROR "MATPLOT_PACKAGE_ROOT is required")
endif()

add_executable(matplotplusplus-consumer main.cpp)
target_compile_features(matplotplusplus-consumer PRIVATE cxx_std_23)
target_include_directories(matplotplusplus-consumer PRIVATE "${MATPLOT_PACKAGE_ROOT}/include")

# The upstream Matplot++ package installs its import library directly in lib/.
# Other BuildEngine packages may use configuration-specific lib directories.
find_library(MATPLOT_IMPORT
   NAMES matplot matplotplusplus libmatplot libmatplotplusplus
   PATHS
      "${MATPLOT_PACKAGE_ROOT}/lib"
      "${MATPLOT_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}"
   NO_DEFAULT_PATH REQUIRED)

# Some upstream builds install a separate nodesoup archive; other shared
# builds embed it in the Matplot++ DLL. Do not require an independently
# published archive when the managed package does not contain one.
find_library(MATPLOT_NODESOUP
   NAMES nodesoup libnodesoup
   PATHS
      "${MATPLOT_PACKAGE_ROOT}/lib/Matplot++"
      "${MATPLOT_PACKAGE_ROOT}/lib"
      "${MATPLOT_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}"
      "${MATPLOT_PACKAGE_ROOT}/lib/win64/${CMAKE_BUILD_TYPE}/Matplot++"
   NO_DEFAULT_PATH)

target_link_libraries(matplotplusplus-consumer PRIVATE "${MATPLOT_IMPORT}")
if(MATPLOT_NODESOUP)
   target_link_libraries(matplotplusplus-consumer PRIVATE "${MATPLOT_NODESOUP}")
endif()
