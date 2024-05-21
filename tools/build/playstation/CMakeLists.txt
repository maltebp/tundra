cmake_minimum_required(VERSION 3.21)

include(../tundra.cmake)

set( CMAKE_MAKE_PROGRAM "${TD_DIR_EXTERNAL}/psn00bsdk/bin/ninja.exe" CACHE FILEPATH "" )
set( CMAKE_TOOLCHAIN_FILE ${TD_DIR_EXTERNAL}/psn00bsdk/lib/libpsn00b/cmake/sdk.cmake  )
set( PSN00BSDK_TC ${TD_DIR_EXTERNAL}/psn00bsdk/bin  )

set( _TD_MODEL_COMPILER_PATH ${TD_DIR_ROOT}/build/developer/bin/Debug/td-asset-compiler.exe)
include(../../cmake/internal/compile-asset.cmake)

project(
	td.playstation
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  ""
)

# Generate the .clangd file in the root
include(../../cmake/internal/generate-clangd.cmake)

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

add_subdirectory(tests/benchmarks/entity-system/iterate-no-holes)
add_subdirectory(tests/benchmarks/entity-system/iterate-interleaved-holes)
add_subdirectory(tests/benchmarks/entity-system/iterate-big-holes)
add_subdirectory(tests/benchmarks/entity-system/construct-empty)
add_subdirectory(tests/benchmarks/entity-system/construct-reserved)
add_subdirectory(tests/benchmarks/entity-system/construct-in-holes)
add_subdirectory(tests/benchmarks/entity-system/construct-with-siblings)
add_subdirectory(tests/benchmarks/entity-system/destroy-empty)
add_subdirectory(tests/benchmarks/entity-system/destroy-with-siblings)
add_subdirectory(tests/benchmarks/entity-system/destroy-with-holes)
add_subdirectory(tests/benchmarks/transforms/construct-top-down)
add_subdirectory(tests/benchmarks/transforms/construct-bottom-up)
add_subdirectory(tests/benchmarks/transforms/destroy-top-down)
add_subdirectory(tests/benchmarks/transforms/destroy-bottom-up)
add_subdirectory(tests/benchmarks/transforms/update-root)
add_subdirectory(tests/benchmarks/transforms/compute-no-changes)
add_subdirectory(tests/benchmarks/transforms/compute-all-dirty)
add_subdirectory(tests/benchmarks/transforms/compute-all-dirty-bottom-to-top)
add_subdirectory(tests/benchmarks/model-rendering/flat-color)
add_subdirectory(tests/benchmarks/model-rendering/out-of-view)
add_subdirectory(tests/benchmarks/sprite-rendering/textures)
add_subdirectory(tests/benchmarks/text-rendering/long)
