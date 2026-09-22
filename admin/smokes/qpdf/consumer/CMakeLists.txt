cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeQpdf LANGUAGES CXX)

find_package(qpdf CONFIG REQUIRED)

add_executable(qpdf-consumer main.cpp)
set_property(TARGET qpdf-consumer PROPERTY CXX_SCAN_FOR_MODULES OFF)
target_compile_features(qpdf-consumer PRIVATE cxx_std_23)
target_link_libraries(qpdf-consumer PRIVATE qpdf::libqpdf)
