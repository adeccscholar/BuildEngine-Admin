cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeXercesC LANGUAGES CXX)

find_package(XercesC CONFIG REQUIRED)

add_executable(xerces-c-consumer main.cpp)
set_property(TARGET xerces-c-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(xerces-c-consumer PRIVATE cxx_std_23)
target_link_libraries(xerces-c-consumer PRIVATE XercesC::XercesC)
