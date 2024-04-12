cmake_minimum_required(VERSION 3.21)

set( TD_TESTS_SOUND_DIR_SRC ${TD_DIR_TESTS}/manual/playstation/sound )

set( TD_TESTS_INPUT_SOURCES
	${TD_TESTS_SOUND_DIR_SRC}/main.cpp
)

psn00bsdk_add_executable(td.playstation.tests.manual.sound GPREL ${TD_TESTS_INPUT_SOURCES})

psn00bsdk_target_incbin(td.playstation.tests.manual.sound PRIVATE snd_jump ${TD_DIR_TESTS}/data/jump.vag)
psn00bsdk_target_incbin(td.playstation.tests.manual.sound PRIVATE snd_drone_engine ${TD_DIR_TESTS}/data/drone-engine.vag)
psn00bsdk_target_incbin(td.playstation.tests.manual.sound PRIVATE snd_techno_music ${TD_DIR_TESTS}/data/techno-music.vag)

target_link_libraries(td.playstation.tests.manual.sound PRIVATE td.base)
target_link_libraries(td.playstation.tests.manual.sound PRIVATE td.playstation.engine)

target_include_directories(
    td.playstation.tests.manual.sound
    PRIVATE ${TD_TESTS_SOUND_DIR_SRC}
)

source_group(TREE ${TD_TESTS_SOUND_DIR_SRC} PREFIX src FILES ${TD_TESTS_INPUT_SOURCES})