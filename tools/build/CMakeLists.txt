cmake_minimum_required(VERSION 3.21)

project(
	tundra
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  ""
)

set( SRC ../../src/tundra )

set( FILES
	${SRC}/test.cpp
)

psn00bsdk_add_library(tundra STATIC ${FILES})

include_directories(../../include/ ../../src/)

set_property(TARGET tundra PROPERTY PSN00BSDK_TARGET_TYPE EXECUTABLE_GPREL)