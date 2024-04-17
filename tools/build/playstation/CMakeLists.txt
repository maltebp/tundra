cmake_minimum_required(VERSION 3.21)

include(../tundra.cmake)

set( CMAKE_MAKE_PROGRAM "${TD_DIR_EXTERNAL}/psn00bsdk/bin/ninja.exe" CACHE FILEPATH "" )
set( CMAKE_TOOLCHAIN_FILE ${TD_DIR_EXTERNAL}/psn00bsdk/lib/libpsn00b/cmake/sdk.cmake  )
set( PSN00BSDK_TC ${TD_DIR_EXTERNAL}/psn00bsdk/bin  )

project(
	td.playstation
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  ""
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set( TD_PLATFORM PlayStation )
add_compile_definitions( TD_PLATFORM_PLAYSTATION )

if (MSVC)
	# Modern pre-processor (e.g. to enable __VA_OPT__) - seemingly can't add this to tundra.cmake
	add_compile_options(/Zc:preprocessor /W4 /WX)
else()
	add_compile_options(-Wconversion -Wall -Wextra -pedantic -Wsign-conversion)
endif() 

add_subdirectory(../base base)
add_subdirectory(engine)
add_subdirectory(tests/automated)
add_subdirectory(tests/manual/input)
add_subdirectory(tests/manual/model-rendering)
add_subdirectory(tests/manual/sound)