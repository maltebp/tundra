cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_DIR_SRC ${TD_DIR_TESTS}/automated/playstation )

set( TD_TESTS_SOURCES
	${TD_TESTS_DIR_SRC}/main.cpp
)

psn00bsdk_add_executable(td.playstation.tests.automated GPREL ${TD_TESTS_SOURCES})

target_link_libraries(td.playstation.tests.automated PRIVATE td.base)
target_link_libraries(td.playstation.tests.automated PRIVATE td.playstation.engine)

target_include_directories(
    td.playstation.tests.automated
    PRIVATE ${TD_DIR_TESTS}/automated/base
    PRIVATE ${TD_TESTS_DIR_SRC}
)

source_group(TREE ${TD_TESTS_DIR_SRC} PREFIX src FILES ${TD_TESTS_SOURCES})