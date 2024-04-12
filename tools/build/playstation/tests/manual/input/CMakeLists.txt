cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_INPUT_DIR_SRC ${TD_DIR_TESTS}/manual/playstation/input )

set( TD_TESTS_INPUT_SOURCES
	${TD_TESTS_INPUT_DIR_SRC}/main.cpp
)

psn00bsdk_add_executable(td.playstation.tests.manual.input GPREL ${TD_TESTS_INPUT_SOURCES})

target_link_libraries(td.playstation.tests.manual.input PRIVATE td.base)
target_link_libraries(td.playstation.tests.manual.input PRIVATE td.playstation.engine)

target_include_directories(
    td.playstation.tests.manual.input
    PRIVATE ${TD_TESTS_INPUT_DIR_SRC}
)

source_group(TREE ${TD_TESTS_INPUT_DIR_SRC} PREFIX src FILES ${TD_TESTS_INPUT_SOURCES})