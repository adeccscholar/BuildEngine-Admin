cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeImPlot LANGUAGES CXX)
if(NOT DEFINED IMPLOT_PACKAGE_ROOT OR NOT DEFINED IMGUI_PACKAGE_ROOT)
   message(FATAL_ERROR "IMPLOT_PACKAGE_ROOT and IMGUI_PACKAGE_ROOT are required")
endif()
add_executable(implot-consumer main.cpp)
target_compile_features(implot-consumer PRIVATE cxx_std_23)
target_compile_definitions(implot-consumer PRIVATE
   "IMGUI_API=__declspec(dllimport)" "IMPLOT_API=__declspec(dllimport)")
target_include_directories(implot-consumer PRIVATE
   "${IMPLOT_PACKAGE_ROOT}/include" "${IMGUI_PACKAGE_ROOT}/include")
target_link_libraries(implot-consumer PRIVATE
   "${IMPLOT_PACKAGE_ROOT}/lib/win64/Release/libimplot.lib"
   "${IMGUI_PACKAGE_ROOT}/lib/win64/Release/libimgui.lib")
