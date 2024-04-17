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

# Generate the .clangd file in the root
string(REPLACE "C:" "" _TD_DIR_EXTERNAL_CLANGD ${TD_DIR_EXTERNAL})
file(CONFIGURE OUTPUT ${TD_DIR_ROOT}/.clangd CONTENT
[[
CompileFlags:
    Add:      [ 
        -ferror-limit=200,
        -march=mips1,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/mipsel-none-elf/include/c++/12.3.0,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/mipsel-none-elf/include/c++/12.3.0/mipsel-none-elf/soft-float,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/lib/gcc/mipsel-none-elf/12.3.0/include,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/lib/gcc/mipsel-none-elf/12.3.0/include-fixed,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/lib/gcc/mipsel-none-elf/c++/12.3.0/include-fixed,
        -isystem, ${_TD_DIR_EXTERNAL_CLANGD}/psn00bsdk/mipsel-none-elf/include
        
    ]
    Remove:   [ -march, -mno-llsc, -mdivide-breaks ]

Diagnostics:
    Suppress: "asm_unknown_register_name"
]]
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