cmake_minimum_required(VERSION 3.21)

set( TD_BASE_DIR_INCLUDE ${TD_DIR_INCLUDE}/base )

set( TD_BASE_HEADERS 
    ${TD_BASE_DIR_INCLUDE}/tundra/core/assert.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/core/grid-allocator.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/core/log.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/core/macro-util.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/core/types.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/core/string.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/core/time.hpp

    ${TD_BASE_DIR_INCLUDE}/tundra/assets/model/model-asset.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/assets/model/model-file-header.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/assets/model/model-deserializer.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/assets/model/model-part-header.hpp
    ${TD_BASE_DIR_INCLUDE}/tundra/assets/model/model-part.hpp

    ${TD_BASE_DIR_INCLUDE}/tundra/assets/texture/texture-asset.hpp
)

set( TD_BASE_DIR_SRC ${TD_DIR_SRC}/base )

set( TD_BASE_SOURCES

    ${TD_BASE_DIR_SRC}/tundra/core/string.cpp
    ${TD_BASE_DIR_SRC}/tundra/core/string-util.cpp
    ${TD_BASE_DIR_SRC}/tundra/core/grid-allocator.cpp

    ${TD_BASE_DIR_SRC}/tundra/assets/model/model-deserializer.cpp

    ${TD_BASE_DIR_SRC}/tundra/engine/entity-system/component-meta-data.cpp
    ${TD_BASE_DIR_SRC}/tundra/engine/entity-system/component-base.cpp

    ${TD_BASE_DIR_SRC}/tundra/engine/dynamic-transform.cpp

    ${TD_BASE_DIR_SRC}/tundra/test-framework/test-runner.cpp
    ${TD_BASE_DIR_SRC}/tundra/test-framework/test.cpp
)

if(TD_PLATFORM STREQUAL "PlayStation")
    psn00bsdk_add_library(td.base STATIC ${TD_BASE_SOURCES} ${TD_BASE_HEADERS})
    set_property(TARGET td.base PROPERTY PSN00BSDK_TARGET_TYPE EXECUTABLE_GPREL)
else()
    add_library(td.base STATIC ${TD_BASE_SOURCES} ${TD_BASE_HEADERS})
endif()


target_include_directories(
    td.base
    PUBLIC ${TD_BASE_DIR_INCLUDE}
    PRIVATE ${TD_BASE_DIR_SRCs}
)

source_group(TREE ${TD_BASE_DIR_SRC}/tundra PREFIX src FILES ${TD_BASE_SOURCES})
source_group(TREE ${TD_BASE_DIR_INCLUDE}/tundra PREFIX include FILES ${TD_BASE_HEADERS})