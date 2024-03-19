cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_DIR_SRC ${TD_DIR_TESTS}/developer )

set( TD_TESTS_SOURCES

	${TD_TESTS_DIR_SRC}/main.cpp
)

add_executable(td.developer.tests ${TD_TESTS_SOURCES})

target_link_libraries(td.developer.tests td.base)
target_link_libraries(td.developer.tests td.developer.asset-compiler)

target_include_directories(
    td.developer.tests
    PRIVATE ${TD_DIR_TESTS}/base
	PRIVATE ${TD_ASSET_COMPILER_DIR_SRC}
)

source_group(TREE ${TD_TESTS_DIR_SRC} PREFIX src FILES ${TD_TESTS_SOURCES})