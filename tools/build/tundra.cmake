cmake_minimum_required(VERSION 3.21)

set( CMAKE_CXX_STANDARD 20 )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_compile_options("$<$<CONFIG:RELEASE>:-DNO_ASSERT>")

get_filename_component(TD_DIR_ROOT ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)
get_filename_component(TD_DIR_SRC ${CMAKE_CURRENT_LIST_DIR}/../../src ABSOLUTE)
get_filename_component(TD_DIR_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/../../include ABSOLUTE)
get_filename_component(TD_DIR_TESTS ${CMAKE_CURRENT_LIST_DIR}/../../tests ABSOLUTE)

