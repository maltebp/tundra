cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_DIR_SRC ${TD_DIR_TESTS}/manual/playstation/model-rendering )

set( TD_TESTS_SOURCES
	${TD_TESTS_DIR_SRC}/main.cpp
)

psn00bsdk_add_executable(td.playstation.tests.manual.model-rendering GPREL ${TD_TESTS_SOURCES})

td_target_assets(td.playstation.tests.manual.model-rendering ${TD_DIR_TESTS}/data 
    MODEL mdl_car models/car.obj
    TEXTURE tex_mdl_car_1 models/car.png
    MODEL mdl_sphere_box models/sphere-box.obj
    MODEL mdl_sphere models/sphere.obj
    MODEL mdl_fish models/fish.obj
    MODEL mdl_car models/car.obj
    TEXTURE tex_ball textures/circle.png PALETTE4
    TEXTURE tex_dog textures/herman.png PALETTE8
)

target_link_libraries(td.playstation.tests.manual.model-rendering PRIVATE td.base)
target_link_libraries(td.playstation.tests.manual.model-rendering PRIVATE td.playstation.engine)

target_include_directories(
    td.playstation.tests.manual.model-rendering
    PRIVATE ${TD_TESTS_DIR_SRC}
)

source_group(TREE ${TD_TESTS_DIR_SRC} PREFIX src FILES ${TD_TESTS_SOURCES})