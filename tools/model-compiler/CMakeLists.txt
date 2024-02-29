cmake_minimum_required(VERSION 3.21)

project(
	td-model-compiler
	LANGUAGES    CXX
	VERSION      1.0.0
	DESCRIPTION  ""
)


set( SRC ../../src/tundra )

set( FILES
	${SRC}/asset-compilers/model-compiler/main.cpp
)

add_executable(td-model-compiler ${FILES})

set_target_properties(td-model-compiler PROPERTIES CXX_STANDARD 20)


include_directories(../../src/)