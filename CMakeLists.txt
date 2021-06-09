cmake_minimum_required(VERSION 3.10)
project(bflc VERSION 0.1.0)
include(CheckSymbolExists)

# output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# blfc library
set(LIB_DIRECTORY libbflc)
file(GLOB_RECURSE LIB_SOURCES ${LIB_DIRECTORY}/*.c)
add_library(bflc_obj OBJECT ${LIB_SOURCES})
set_property(TARGET bflc_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(bflc_shared SHARED $<TARGET_OBJECTS:bflc_obj>)
add_library(bflc_static STATIC $<TARGET_OBJECTS:bflc_obj>)

# bflc compiler
set(BIN_DIRECTORY bflc)
aux_source_directory(${BIN_DIRECTORY} BIN_SOURCES)
add_executable(bflc ${BIN_SOURCES})
target_link_libraries(bflc PRIVATE bflc_static)
target_include_directories(bflc PRIVATE .)

# jit example
check_symbol_exists(mmap "sys/mman.h" HAVE_MMAP)
check_symbol_exists(munmap "sys/mman.h" HAVE_MUNMAP)

if(HAVE_MMAP AND HAVE_MUNMAP)
    add_executable(jit_example docs/jit_example.c)
    target_link_libraries(jit_example PRIVATE bflc_static)
    target_include_directories(jit_example PRIVATE ${LIB_DIRECTORY})
endif()
