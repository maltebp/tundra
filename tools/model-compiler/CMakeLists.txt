cmake_minimum_required(VERSION 3.21)

project(
	td-model-compiler
	LANGUAGES    CXX
	VERSION      1.0.0
	DESCRIPTION  ""
)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_compile_definitions(TD_PLATFORM_DEVELOPMENT)

set( SRC ../../src/tundra )
set( SRC_AC ${SRC}/asset-compilers/model-compiler )

set( FILES
	${SRC}/core/assert.hpp
	${SRC_AC}/main.cpp
	${SRC_AC}/assert.hpp
	${SRC_AC}/float2.hpp
	${SRC_AC}/float3.hpp
	${SRC_AC}/int3.hpp
	${SRC_AC}/obj-model.hpp
	${SRC_AC}/obj-object.hpp
	${SRC_AC}/obj-parser.cpp
	${SRC_AC}/obj-parser.hpp
)

add_executable(td-model-compiler ${FILES})

set_target_properties(td-model-compiler PROPERTIES CXX_STANDARD 20)


include_directories(../../src/)