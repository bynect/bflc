cmake_minimum_required(VERSION 3.10)
project(bflc VERSION 2.0)
include(CheckSymbolExists)

# Set variables
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(LIBRARY_DIR lib)
set(COMPILER_DIR src)
set(BACK_DIR back)
set(FRONT_DIR front)
set(OPT_DIR opt)

# Global settings
add_compile_definitions(BFLC_VERSION="${CMAKE_PROJECT_VERSION}")
set_property(GLOBAL PROPERTY C_STANDARD 99)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)
endif()

# Build bflc library (libbflc.a)
aux_source_directory(${LIBRARY_DIR} LIBRARY_SRC)
add_library(bflc STATIC ${LIBRARY_SRC})

# Build bflc compiler (bflc)
aux_source_directory(${COMPILER_DIR} COMPILER_SRC)
aux_source_directory(${FRONT_DIR} FRONT_SRC)
aux_source_directory(${BACK_DIR} BACK_SRC)
list(APPEND COMPILER_SRC ${FRONT_SRC} ${BACK_SRC})

add_executable(bflc_bin ${COMPILER_SRC})
set_target_properties(bflc_bin PROPERTIES OUTPUT_NAME bflc)

target_link_libraries(bflc_bin PRIVATE bflc)
target_include_directories(bflc_bin PRIVATE .)
target_include_directories(bflc_bin PRIVATE ${LIBRARY_DIR})

add_dependencies(bflc_bin opt)
target_link_libraries(bflc_bin PRIVATE opt)
target_include_directories(bflc_bin PRIVATE ${OPT_DIR})

# Build opt library (libopt.a)
set(LIBOPT ${PROJECT_BINARY_DIR}/libopt.a)
add_custom_command(
	OUTPUT ${LIBOPT}
	COMMAND make LIB=${LIBOPT}
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/${OPT_DIR}
)

add_custom_target(opt_dummy DEPENDS ${LIBOPT})
add_library(opt STATIC IMPORTED GLOBAL)
add_dependencies(opt opt_dummy)

set_target_properties(opt
	PROPERTIES
	IMPORTED_LOCATION ${LIBOPT}
	INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/${OPT_DIR}
)

# Build jit example
check_symbol_exists(mmap "sys/mman.h" WITH_MMAP)
check_symbol_exists(munmap "sys/mman.h" WITH_MUNMAP)
check_symbol_exists(mremap "sys/mman.h" WITH_MREMAP)

if(WITH_MMAP AND WITH_MUNMAP)
	list(APPEND EXAMPLE_SRC jit/example.c)
	aux_source_directory(${FRONT_DIR} FRONT_SRC)
	aux_source_directory(${BACK_DIR} BACK_SRC)
	list(APPEND EXAMPLE_SRC ${FRONT_SRC} ${BACK_SRC})

	add_executable(jit_example ${EXAMPLE_SRC})
	target_link_libraries(jit_example PRIVATE bflc)
	target_include_directories(jit_example PRIVATE .)
	target_include_directories(jit_example PRIVATE ${LIBRARY_DIR})
endif()
