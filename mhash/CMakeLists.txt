cmake_minimum_required(VERSION 3.14.4)
project(mhash)

set (CMAKE_C_STANDARD 11)

file(
	GLOB mhash_lib_sources lib/*.c lib/*.h
	include/mutils/mglobal.h include/mutils/mhash.h include/mutils/mincludes.h
	include/mutils/mtypes.h include/mutils/mutils.h
)
add_library(mhash ${mhash_lib_sources})

if(WIN32)
	set(mhash_config_source include/mutils/mhash_config.win32.h)
elseif(APPLE)
	set(mhash_config_source include/mutils/mhash_config.osx.h)
elseif(UNIX)
	set(mhash_config_source include/mutils/mhash_config.linux.h)
endif()

configure_file(${mhash_config_source} mutils/mhash_config.h COPYONLY)
target_sources(mhash PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/mutils/mhash_config.h)
target_include_directories(
	mhash
	PUBLIC
		${CMAKE_CURRENT_BINARY_DIR}
		include
	PRIVATE
		lib
)
