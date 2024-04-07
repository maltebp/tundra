cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_DIR_SRC ${TD_DIR_TESTS}/manual/playstation/model-rendering )

set( TD_TESTS_SOURCES
	${TD_TESTS_DIR_SRC}/main.cpp
)

psn00bsdk_add_executable(td.playstation.tests.manual.model-rendering GPREL ${TD_TESTS_SOURCES})

psn00bsdk_target_incbin(td.playstation.tests.manual.model-rendering PRIVATE mdl_sphere ${TD_DIR_TESTS}/data/sphere.td_model)
psn00bsdk_target_incbin(td.playstation.tests.manual.model-rendering PRIVATE mdl_sphere_box ${TD_DIR_TESTS}/data/sphere-box.td_model)
psn00bsdk_target_incbin(td.playstation.tests.manual.model-rendering PRIVATE mdl_fish ${TD_DIR_TESTS}/data/fish.td_model)

target_link_libraries(td.playstation.tests.manual.model-rendering PRIVATE td.base)
target_link_libraries(td.playstation.tests.manual.model-rendering PRIVATE td.playstation.engine)

target_include_directories(
    td.playstation.tests.manual.model-rendering
    PRIVATE ${TD_TESTS_DIR_SRC}
)

source_group(TREE ${TD_TESTS_DIR_SRC} PREFIX src FILES ${TD_TESTS_SOURCES})