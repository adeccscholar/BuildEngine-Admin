cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeVTK LANGUAGES CXX)

find_package(VTK 9.6 CONFIG REQUIRED COMPONENTS CommonCore CommonDataModel FiltersCore)

add_executable(vtk-consumer main.cpp)
set_property(TARGET vtk-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(vtk-consumer PRIVATE cxx_std_23)
target_link_libraries(vtk-consumer PRIVATE
   VTK::CommonCore
   VTK::CommonDataModel
   VTK::FiltersCore
)
