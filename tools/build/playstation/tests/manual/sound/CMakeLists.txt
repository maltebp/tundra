cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_SOUND_DIR_SRC ${TD_DIR_TESTS}/manual/playstation/sound )

set( TD_TESTS_INPUT_SOURCES
	${TD_TESTS_SOUND_DIR_SRC}/main.cpp
)

psn00bsdk_add_executable(td.playstation.tests.manual.sound GPREL ${TD_TESTS_INPUT_SOURCES})

td_target_assets(td.playstation.tests.manual.sound ${TD_DIR_TESTS}/data 
    SOUND snd_jump sounds/jump.wav
    SOUND snd_drone_engine sounds/drone-engine.wav LOOP
    SOUND snd_techno_music sounds/techno-music.wav LOOP
)

target_link_libraries(td.playstation.tests.manual.sound PRIVATE td.base)
target_link_libraries(td.playstation.tests.manual.sound PRIVATE td.playstation.engine)

target_include_directories(
    td.playstation.tests.manual.sound
    PRIVATE ${TD_TESTS_SOUND_DIR_SRC}
)

source_group(TREE ${TD_TESTS_SOUND_DIR_SRC} PREFIX src FILES ${TD_TESTS_INPUT_SOURCES})