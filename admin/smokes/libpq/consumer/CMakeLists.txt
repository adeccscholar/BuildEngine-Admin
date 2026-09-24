cmake_minimum_required(VERSION 3.25)

project(libpq_consumer LANGUAGES C)

if(NOT PostgreSQL_INCLUDE_DIR)
   message(FATAL_ERROR "PostgreSQL_INCLUDE_DIR is required")
endif()

if(NOT PostgreSQL_LIBRARY)
   message(FATAL_ERROR "PostgreSQL_LIBRARY is required")
endif()

add_executable(libpq-consumer main.c)
target_include_directories(libpq-consumer PRIVATE "${PostgreSQL_INCLUDE_DIR}")
target_link_libraries(libpq-consumer PRIVATE "${PostgreSQL_LIBRARY}")
