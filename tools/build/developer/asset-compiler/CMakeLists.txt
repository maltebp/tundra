cmake_minimum_required(VERSION 3.21)

set( TD_ASSET_COMPILER_DIR_SRC ${TD_DIR_SRC}/developer/asset-compiler )

set( TD_ASSET_COMPILER_SOURCES

	${TD_ASSET_COMPILER_DIR_SRC}/tundra/main.cpp

	${TD_ASSET_COMPILER_DIR_SRC}/tundra/assert-input.hpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/float2.hpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/float3.hpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/int3.hpp

	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/model-serializer.cpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/model-serializer.hpp

	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/obj/obj-face.hpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/obj/obj-face.hpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/obj/obj-model.cpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/obj/obj-model.hpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/obj/obj-object-part.hpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/obj/obj-object.hpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/obj/obj-parser.cpp
	${TD_ASSET_COMPILER_DIR_SRC}/tundra/model/obj/obj-parser.hpp

)

add_executable(td.developer.asset-compiler ${TD_ASSET_COMPILER_SOURCES})

target_link_libraries(td.developer.asset-compiler td.base)

target_include_directories(
    td.developer.asset-compiler
    PRIVATE ${TD_ASSET_COMPILER_DIR_SRC}
)

source_group(TREE ${TD_ASSET_COMPILER_DIR_SRC}/tundra PREFIX src FILES ${TD_ASSET_COMPILER_SOURCES})