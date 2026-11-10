cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeVTK972Rendering LANGUAGES CXX)
find_package(VTK 9.7.2 EXACT CONFIG REQUIRED COMPONENTS
   CommonCore CommonDataModel FiltersCore FiltersSources IOImage
   RenderingCore RenderingOpenGL2 RenderingFreeType
   RenderingVolume RenderingVolumeOpenGL2 RenderingAnnotation InteractionStyle)
add_executable(vtk-rendering-smoke main.cpp)
set_property(TARGET vtk-rendering-smoke PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(vtk-rendering-smoke PRIVATE cxx_std_23)
target_link_libraries(vtk-rendering-smoke PRIVATE
   VTK::CommonCore VTK::CommonDataModel VTK::FiltersCore VTK::FiltersSources
   VTK::IOImage VTK::RenderingCore VTK::RenderingOpenGL2 VTK::RenderingFreeType
   VTK::RenderingVolume VTK::RenderingVolumeOpenGL2 VTK::RenderingAnnotation
   VTK::InteractionStyle)
vtk_module_autoinit(TARGETS vtk-rendering-smoke MODULES
   VTK::RenderingCore VTK::RenderingOpenGL2 VTK::RenderingFreeType
   VTK::RenderingVolume VTK::RenderingVolumeOpenGL2 VTK::InteractionStyle)
