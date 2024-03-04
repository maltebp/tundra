cmake_minimum_required(VERSION 3.21)

set( TD_ENGINE_DIR_INCLUDE ${TD_DIR_INCLUDE}/playstation/engine )

set( TD_ENGINE_HEADERS 
    ${TD_ENGINE_DIR_INCLUDE}/tundra/temp.hpp
)

set( TD_ENGINE_DIR_SRC ${TD_DIR_SRC}/playstation/engine )

set( TD_ENGINE_SOURCES
	${TD_ENGINE_DIR_SRC}/tundra/temp.cpp
)

psn00bsdk_add_library(td.playstation.engine STATIC ${TD_ENGINE_SOURCES} ${TD_ENGINE_HEADERS})
set_property(TARGET td.playstation.engine PROPERTY PSN00BSDK_TARGET_TYPE EXECUTABLE_GPREL)

target_link_libraries(td.playstation.engine td.base)

target_include_directories(
    td.playstation.engine
	PUBLIC ${TD_ENGINE_DIR_INCLUDE}
    PRIVATE ${TD_ENGINE_DIR_SRC}
)

source_group(TREE ${TD_ENGINE_DIR_INCLUDE}/tundra PREFIX include FILES ${TD_ENGINE_HEADERS})
source_group(TREE ${TD_ENGINE_DIR_SRC}/tundra PREFIX src FILES ${TD_ENGINE_SOURCES})