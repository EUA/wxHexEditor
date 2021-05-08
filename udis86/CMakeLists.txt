cmake_minimum_required(VERSION 3.14.4)
project(udis86)

set (CMAKE_C_STANDARD 11)

file(GLOB udis_lib_sources libudis86/*.c libudis86/*.h)
add_library(udis ${udis_lib_sources})

# Generate a file using Python - https://stackoverflow.com/questions/49018868/
find_package(Python REQUIRED)
make_directory(${CMAKE_BINARY_DIR}/libudis86)
add_custom_target(
 run ALL
 COMMAND ${Python_EXECUTABLE} scripts/ud_itab.py ${CMAKE_CURRENT_SOURCE_DIR}/docs/x86/optable.xml ${CMAKE_BINARY_DIR}/libudis86
 BYPRODUCTS ${CMAKE_BINARY_DIR}/libudis86/itab.h ${CMAKE_BINARY_DIR}/libudis86/itab.c
 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
 COMMENT "Generating itab"
)
add_dependencies(udis run)
target_sources(
	udis PRIVATE
	${CMAKE_BINARY_DIR}/libudis86/itab.c ${CMAKE_BINARY_DIR}/libudis86/itab.h
)

target_include_directories(
	udis
	PUBLIC ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/libudis86
	PRIVATE libudis86
)
