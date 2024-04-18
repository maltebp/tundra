
if( NOT TUNDRA_DIR )
    message(FATAL_ERROR "TUNDRA_DIR is not set (must be set to directroy containing TUNDRA)")
endif()

if( NOT EXISTS ${TUNDRA_DIR} )
    message(FATAL_ERROR "Cannot find TUNDRA_DIR '${TUNDRA_DIR}'")
endif()

if( NOT EXISTS ${TD_GAME_DIR_ROOT} )
    message(FATAL_ERROR "TD_GAME_DIR_ROOT is not set")
endif()

get_filename_component(TD_DIR_ROOT ${TUNDRA_DIR} ABSOLUTE)
get_filename_component(TD_DIR_INCLUDE ${TUNDRA_DIR}/include ABSOLUTE)
get_filename_component(TD_DIR_EXTERNAL ${TUNDRA_DIR}/external ABSOLUTE) 

set(_TD_MODEL_COMPILER_PATH ${TUNDRA_DIR}/bin/release/td-asset-compiler.exe)

# Some generatl setup
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
add_compile_options("$<$<CONFIG:RELEASE>:-DNO_ASSERT>")

# Setup for psn00bsdk
set( CMAKE_MAKE_PROGRAM "${TD_DIR_EXTERNAL}/psn00bsdk/bin/ninja.exe" CACHE FILEPATH "" )
set( CMAKE_TOOLCHAIN_FILE ${TD_DIR_EXTERNAL}/psn00bsdk/lib/libpsn00b/cmake/sdk.cmake  )
set( PSN00BSDK_TC ${TD_DIR_EXTERNAL}/psn00bsdk/bin  )

include(${CMAKE_CURRENT_LIST_DIR}/internal/compile-asset.cmake)

# Generate clangd
include(${CMAKE_CURRENT_LIST_DIR}/internal/generate-clangd.cmake)
_td_generate_clangd_config(${TD_GAME_DIR_ROOT})

# This is by no means a proper setup function - I just wanted to move all the Tundra setup out of the
# project CMakeLists.txt and this will do for now
function(td_link_engine target) 

    set_target_properties(${target} PROPERTIES CXX_STANDARD 20)

    add_compile_definitions(TD_PLATFORM_PLAYSTATION)

    if (MSVC)
        # Modern pre-processor (e.g. to enable __VA_OPT__) - seemingly can't add this to tundra.cmake
        target_compile_options(${target} PUBLIC /Zc:preprocessor /W4 /WX)
    else()
        target_compile_options(${target} PUBLIC -Wconversion -Wall -Wextra -Wpedantic -Wsign-conversion)
    endif() 

    get_filename_component( TD_INCLUDE ${TUNDRA_DIR}/include ABSOLUTE)
    get_filename_component( TD_LIB ${TUNDRA_DIR}/lib ABSOLUTE)

    target_include_directories(
        ${target}
        PRIVATE ${TD_INCLUDE}/base
        PRIVATE ${TD_INCLUDE}/playstation/engine
    )

    add_library(td-engine-debug STATIC IMPORTED)
    set_target_properties(td-engine-debug PROPERTIES IMPORTED_LOCATION ${TD_LIB}/Debug/libtd.playstation.engine.a )

    add_library(td-engine-release STATIC IMPORTED)
    set_target_properties(td-engine-release PROPERTIES IMPORTED_LOCATION ${TD_LIB}/Release/libtd.playstation.engine.a )

    add_library(td-base-debug STATIC IMPORTED)
    set_target_properties(td-base-debug PROPERTIES IMPORTED_LOCATION ${TD_LIB}/Debug/libtd.base.a )

    add_library(td-base-release STATIC IMPORTED)
    set_target_properties(td-base-release PROPERTIES IMPORTED_LOCATION ${TD_LIB}/Release/libtd.base.a )

    target_link_libraries(${target} PRIVATE debug td-engine-debug td-base-debug)
    target_link_libraries(${target} PRIVATE optimized td-engine-release td-base-release)

endfunction()