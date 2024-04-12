cmake_minimum_required(VERSION 3.21)

set( TD_ENGINE_DIR_INCLUDE ${TD_DIR_INCLUDE}/playstation/engine )

set( TD_ENGINE_HEADERS 
    ${TD_ENGINE_DIR_INCLUDE}/tundra/assets/texture/texture-loader.hpp

    ${TD_ENGINE_DIR_INCLUDE}/tundra/gte/initialize.hpp
    ${TD_ENGINE_DIR_INCLUDE}/tundra/gte/operations.hpp
    ${TD_ENGINE_DIR_INCLUDE}/tundra/gte/compute-transform.hpp
    ${TD_ENGINE_DIR_INCLUDE}/tundra/playstation-time.hpp

    ${TD_ENGINE_DIR_INCLUDE}/tundra/rendering/primitive-buffer.hpp
    ${TD_ENGINE_DIR_INCLUDE}/tundra/rendering/camera.hpp
    ${TD_ENGINE_DIR_INCLUDE}/tundra/rendering/ordering-table-node.hpp
    ${TD_ENGINE_DIR_INCLUDE}/tundra/rendering/ordering-table-layer.hpp
    ${TD_ENGINE_DIR_INCLUDE}/tundra/rendering/ordering-table.hpp
    ${TD_ENGINE_DIR_INCLUDE}/tundra/rendering/render-system.hpp
)

set( TD_ENGINE_DIR_SRC ${TD_DIR_SRC}/playstation/engine )

set( TD_ENGINE_SOURCES

    ${TD_ENGINE_DIR_SRC}/tundra/input/playstation-input.cpp
    
    ${TD_ENGINE_DIR_SRC}/tundra/assets/texture/texture-loader.cpp
    ${TD_ENGINE_DIR_SRC}/tundra/assets/model/model-asset.cpp

    ${TD_ENGINE_DIR_SRC}/tundra/gte/initialize.cpp 
    ${TD_ENGINE_DIR_SRC}/tundra/gte/operations.cpp 
    ${TD_ENGINE_DIR_SRC}/tundra/gte/compute-transform.cpp
    
    ${TD_ENGINE_DIR_SRC}/tundra/playstation-time.cpp

    ${TD_ENGINE_DIR_SRC}/tundra/rendering/camera.cpp
    ${TD_ENGINE_DIR_SRC}/tundra/rendering/primitive-buffer.cpp
    ${TD_ENGINE_DIR_SRC}/tundra/rendering/ordering-table-layer.cpp
    ${TD_ENGINE_DIR_SRC}/tundra/rendering/ordering-table.cpp
    ${TD_ENGINE_DIR_SRC}/tundra/rendering/render-system.cpp 
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