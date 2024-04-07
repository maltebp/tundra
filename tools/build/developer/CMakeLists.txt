cmake_minimum_required(VERSION 3.21)

include(../tundra.cmake)

project(
	td.developer
	LANGUAGES    C CXX
	VERSION      1.0.0
	DESCRIPTION  ""
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (MSVC)
    # Modern pre-processor (e.g. to enable __VA_OPT__) - seemingly can't add this to tundra.cmake
    add_compile_options(
		/Zc:preprocessor 
		/W4
		/wd4458 # Suppress warning abou variables hiding members
	)
else()
	add_compile_options(-Wconversion -Wall -Wextra -pedantic -Wsign-conversion)
endif() 

set( TD_PLATFORM Developer )
add_compile_definitions( TD_PLATFORM_DEVELOPER )

add_subdirectory(../base base)
add_subdirectory(asset-compiler)
add_subdirectory(tests/automated)