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

    get_filename_component( TD_DIR external/tundra ABSOLUTE)
    get_filename_component( TD_INCLUDE ${TD_DIR}/include ABSOLUTE)
    get_filename_component( TD_LIB ${TD_DIR}/build/playstation/lib ABSOLUTE)

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

    target_link_libraries(tundra-sandbox PRIVATE debug td-engine-debug td-base-debug)
    target_link_libraries(tundra-sandbox PRIVATE optimized td-engine-release td-base-release)

endfunction()