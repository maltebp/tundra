cmake_minimum_required(VERSION 3.21)

include(../tundra.cmake)

project(
	td.playstation
	LANGUAGES    C CXX
	VERSION      1.0.0
	DESCRIPTION  ""
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set( TD_PLATFORM PlayStation )
add_compile_definitions( TD_PLATFORM_PLAYSTATION )

add_subdirectory(../base base)
add_subdirectory(engine)
add_subdirectory(tests)