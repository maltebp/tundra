cmake_minimum_required(VERSION 3.21)

project(
	td-tests
	LANGUAGES    CXX
	VERSION      1.0.0
	DESCRIPTION  ""
)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_compile_definitions(TD_PLATFORM_DEVELOPMENT)

set( TD_INCLUDE ../../include/tundra)
set( TD_SRC ../../src/tundra )

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

	${TD_SRC}/test-framework/test-runner.cpp
	${TD_SRC}/test-framework/test.cpp
	../../tests/main.cpp
)

add_executable(td-tests ${FILES})

set_target_properties(td-tests PROPERTIES CXX_STANDARD 20)


include_directories(../../include/ ../../src/ ../../tests/)