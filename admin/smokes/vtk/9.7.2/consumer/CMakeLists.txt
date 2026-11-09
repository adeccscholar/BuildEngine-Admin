cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeVTK972 LANGUAGES CXX)

# This consumer validates the installed/published 9.7.2 package, not the source tree.
find_package(VTK 9.7.2 EXACT CONFIG REQUIRED
   COMPONENTS CommonCore CommonDataModel FiltersCore CommonCache)

add_executable(vtk-consumer main.cpp)
set_property(TARGET vtk-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(vtk-consumer PRIVATE cxx_std_23)
target_link_libraries(vtk-consumer PRIVATE
   VTK::CommonCore
   VTK::CommonDataModel
   VTK::FiltersCore
   VTK::CommonCache
)
