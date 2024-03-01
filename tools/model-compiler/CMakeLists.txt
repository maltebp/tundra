cmake_minimum_required(VERSION 3.21)

project(
	td-model-compiler
	LANGUAGES    CXX
	VERSION      1.0.0
	DESCRIPTION  ""
)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_compile_definitions(TD_PLATFORM_DEVELOPMENT)

set( TD_INCLUDE ../../include/tundra)
set( TD_SRC ../../src/tundra )
set( SRC_AC ${TD_SRC}/asset-compilers/model-compiler )

set( FILES

	${TD_INCLUDE}/core/assert.hpp
	${TD_INCLUDE}/core/types.hpp
	${TD_INCLUDE}/core/vector.hpp
	${TD_INCLUDE}/assets/model/model-asset.hpp
	${TD_INCLUDE}/assets/model/model-part.hpp
	${TD_INCLUDE}/assets/model/model-deserializer.hpp
	${TD_INCLUDE}/assets/model/model-file-header.hpp
	${TD_INCLUDE}/assets/model/model-part-header.hpp

	${TD_SRC}/engine/assets/model/model-deserializer.cpp
	${TD_SRC}/engine/assets/model/model-asset.cpp

	${SRC_AC}/main.cpp
	${SRC_AC}/assert-input.hpp
	${SRC_AC}/float2.hpp
	${SRC_AC}/float3.hpp
	${SRC_AC}/int3.hpp
	${SRC_AC}/obj-model.cpp
	${SRC_AC}/obj-model.hpp
	${SRC_AC}/obj-object.hpp
	${SRC_AC}/obj-object-part.hpp
	${SRC_AC}/obj-face.hpp
	${SRC_AC}/obj-parser.cpp
	${SRC_AC}/obj-parser.hpp
	${SRC_AC}/model-serializer.cpp
	${SRC_AC}/model-serializer.hpp
)

add_executable(td-model-compiler ${FILES})

set_target_properties(td-model-compiler PROPERTIES CXX_STANDARD 20)


include_directories(../../include/ ../../src/)