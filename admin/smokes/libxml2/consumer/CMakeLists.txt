cmake_minimum_required(VERSION 3.25)
project(BuildEngineSmokeLibXml2 LANGUAGES C)

find_package(LibXml2 CONFIG REQUIRED)

add_executable(libxml2-consumer main.c)
target_link_libraries(libxml2-consumer PRIVATE LibXml2::LibXml2)
