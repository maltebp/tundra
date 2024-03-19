cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_DIR_SRC ${TD_DIR_TESTS}/playstation )

set( TD_TESTS_SOURCES
	${TD_TESTS_DIR_SRC}/main.cpp
)

psn00bsdk_add_executable(td.playstation.tests GPREL ${TD_TESTS_SOURCES})

target_link_libraries(td.playstation.tests PRIVATE td.base)
target_link_libraries(td.playstation.tests PRIVATE td.playstation.engine)

target_include_directories(
    td.playstation.tests
    PRIVATE ${TD_DIR_TESTS}/base
    PRIVATE ${TD_TESTS_DIR_SRC}
)

source_group(TREE ${TD_TESTS_DIR_SRC} PREFIX src FILES ${TD_TESTS_SOURCES})