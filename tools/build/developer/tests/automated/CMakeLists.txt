cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_DIR_SRC ${TD_DIR_TESTS}/automated/developer )

# TODO: Maybe move this to a base cmakelists?
set( TD_BASE_TESTS_DIR ${TD_DIR_TESTS}/automated/base )

set( TD_BASE_TESTS_HEADERS 
    ${TD_BASE_TESTS_DIR}/core/fixed.tests.hpp
    ${TD_BASE_TESTS_DIR}/core/list.tests.hpp
    ${TD_BASE_TESTS_DIR}/core/mat3x3.tests.hpp
    ${TD_BASE_TESTS_DIR}/core/math.tests.hpp
    ${TD_BASE_TESTS_DIR}/core/string-util.tests.hpp
    ${TD_BASE_TESTS_DIR}/core/string.tests.hpp
    ${TD_BASE_TESTS_DIR}/core/vec2.tests.hpp
    ${TD_BASE_TESTS_DIR}/core/vec3.tests.hpp

    ${TD_BASE_TESTS_DIR}/engine/entity-system/chaotic-test.hpp
    ${TD_BASE_TESTS_DIR}/engine/entity-system/component-ref.tests.hpp
    ${TD_BASE_TESTS_DIR}/engine/entity-system/component.tests.hpp
    ${TD_BASE_TESTS_DIR}/engine/entity-system/entity.tests.hpp
    ${TD_BASE_TESTS_DIR}/engine/entity-system/registry-block.tests.hpp
    ${TD_BASE_TESTS_DIR}/engine/entity-system/registry.tests.hpp

    ${TD_BASE_TESTS_DIR}/engine/dynamic-transform.tests.hpp

    ${TD_BASE_TESTS_DIR}/base-tests.hpp
)

set( TD_TESTS_SOURCES
	${TD_TESTS_DIR_SRC}/main.cpp
)

add_executable(td.developer.tests.automated ${TD_TESTS_SOURCES} ${TD_BASE_TESTS_HEADERS})

target_link_libraries(td.developer.tests.automated td.base)
target_link_libraries(td.developer.tests.automated td.developer.asset-compiler)

target_include_directories(
    td.developer.tests.automated
    PRIVATE ${TD_DIR_TESTS}/automated/base
	PRIVATE ${TD_ASSET_COMPILER_DIR_SRC}
)

source_group(TREE ${TD_TESTS_DIR_SRC} PREFIX src FILES ${TD_TESTS_SOURCES})
source_group(TREE ${TD_BASE_TESTS_DIR} PREFIX base FILES ${TD_BASE_TESTS_HEADERS})