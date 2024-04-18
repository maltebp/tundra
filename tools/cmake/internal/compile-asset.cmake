if( NOT EXISTS ${_TD_MODEL_COMPILER_PATH} )
    message(FATAL_ERROR "Cannot find _TD_MODEL_COMPILER_PATH '${_TD_MODEL_COMPILER_PATH}'")
endif()

function(_td_compile_asset_model target asset_name asset_input_path asset_output_path )
    # message("Compiling MODEL name = ${asset_name}, path = ${asset_path}, args: ${ARGN}\n")
    
    list(LENGTH ARGN num_args)
    if( ${num_args} GREATER 0)
        message(WARNING "When compiling ${asset_path}: MODEL compiler takes no args (${num_args} was given)")
    endif()

    cmake_path(REPLACE_EXTENSION asset_output_path LAST_ONLY td_model)
    
    execute_process(COMMAND
        ${_TD_MODEL_COMPILER_PATH}
        ${asset_input_path}
        ${asset_output_path}
        OUTPUT_FILE ${asset_output_path}.log)

    if( NOT EXISTS ${asset_output_path} )
        message(FATAL_ERROR "When compiling ${asset_path}: ouput asset was not created - something went wrong!")
    endif()

    psn00bsdk_target_incbin(${target} PRIVATE ${asset_name} ${asset_output_path})
endfunction()

function(_td_compile_asset_texture asset_directory asset_name asset_input_path asset_output_path)
    # message("Compiling TEXTURE name = ${asset_name}, path = ${asset_path}, args: ${ARGN}")
    
    foreach(arg IN LISTS ARGN)
        if( ${arg} MATCHES PALETTE4 )
            set(texture_color_type -bpp 4)
        elseif( ${arg} MATCHES PALETTE8 )
            set(texture_color_type -bpp 8)
        else()
            message(WARNING "When compiling TEXTURE ${asset_path}: unknown asset parameter ${arg}")
        endif()
    endforeach()

    set(image_compiler_path ${TD_DIR_ROOT}/external/img2tim/bin/img2tim.exe)

    cmake_path(REPLACE_EXTENSION asset_output_path LAST_ONLY td_texture)

    execute_process(COMMAND
        ${image_compiler_path}
        ${asset_input_path} -o ${asset_output_path}
        ${texture_color_type}
        OUTPUT_FILE ${asset_output_path}.log)

    if( NOT EXISTS ${asset_output_path} )
        message(FATAL_ERROR "When compiling ${asset_path}: ouput asset was not created - something went wrong!")
    endif()

    psn00bsdk_target_incbin(${target} PRIVATE ${asset_name} ${asset_output_path})

endfunction()

function(_td_compile_asset_sound asset_directory asset_name asset_input_path asset_output_path)
    # message("Compiling SOUND name = ${asset_name}, path = ${asset_path}, args: ${ARGN}")

    get_filename_component(asset_file_extension ${asset_input_path} LAST_EXT)
    if( NOT ${asset_file_extension} MATCHES "wav")
        message(FATAL_ERROR "When compiling SOUND ${asset_name}: only file .wav files are currently supported")
    endif()

    list(LENGTH ARGN num_args)
    if( ${num_args} GREATER 1)
        message(WARNING "When compiling SOUND ${asset_path}: compiler takes only 1 optional arg (LOOP) (${num_args} was given)")
    endif()

    set(LOOP_VAL "")
    foreach(arg IN LISTS ARGN)
        if( ${arg} MATCHES LOOP )
            set(LOOP_VAL "-L")
        else()
            message(WARNING "When compiling SOUND ${asset_path}: unknown asset parameter ${arg}")
        endif()
    endforeach()

    set(sound_compiler_path ${TD_DIR_ROOT}/external/es-ps2-vag-tool/bin/AIFF2VAG.exe)

    cmake_path(REPLACE_EXTENSION asset_output_path LAST_ONLY td_sound)
    
    execute_process(COMMAND
        ${sound_compiler_path}
        ${asset_input_path}
        ${LOOP_VAL}
        OUTPUT_FILE ${asset_output_path}.log)

    cmake_path(REPLACE_EXTENSION asset_input_path LAST_ONLY .vag OUTPUT_VARIABLE generated_output_path)
    file(RENAME ${generated_output_path} ${asset_output_path} )

    if( NOT EXISTS ${asset_output_path} )
        message(FATAL_ERROR "When compiling ${asset_path}: ouput asset was not created - something went wrong!")
    endif()

    psn00bsdk_target_incbin(${target} PRIVATE ${asset_name} ${asset_output_path})

endfunction()

function(_td_compile_asset target asset_directory)
    list(LENGTH ARGN num_args)
    if( NOT num_args GREATER_EQUAL 3)
        message(FATAL_ERROR "Missing arguments for assets")
    endif()

    list(POP_FRONT ARGN asset_type)
    list(POP_FRONT ARGN asset_name)
    list(POP_FRONT ARGN asset_path)

    set(asset_full_path ${asset_directory}/${asset_path})

    if( NOT EXISTS ${asset_full_path} )
        message(FATAL_ERROR "No asset with path '${asset_full_path}'")
    endif()

    get_target_property(target_binary_dir ${target} BINARY_DIR)
    set(output_path ${target_binary_dir}/assets/${asset_path})
    cmake_path(GET output_path PARENT_PATH output_dir)
    make_directory(${output_dir})

    if( ${asset_type}  MATCHES MODEL )
        _td_compile_asset_model(${target} ${asset_name} ${asset_full_path} ${output_path} ${ARGN})
    elseif(${asset_type} MATCHES TEXTURE)
        _td_compile_asset_texture(${target} ${asset_name} ${asset_full_path} ${output_path} ${ARGN})
    elseif(${asset_type} MATCHES SOUND)
        _td_compile_asset_sound(${target} ${asset_name} ${asset_full_path} ${output_path} ${ARGN})
    else()
        message(FATAL_ERROR "Unknown asset type ${asset_type} (internal error - this should have been caught earlier)")
    endif()

endfunction()

# Asset compilation
function(td_target_assets target asset_directory)
    
    if( NOT TARGET ${target})
        message( FATAL_ERROR ${target} " is not a valid target")
    endif()

    if( NOT EXISTS ${asset_directory} )
        message( FATAL_ERROR "Asset directory " ${asset_directory} " does not exist")
    endif()

    get_property(target_binary_dir TARGET ${target} PROPERTY BINARY_DIR)
    file(REMOVE_RECURSE ${target_binary_dir}/assets)
    file(MAKE_DIRECTORY ${target_binary_dir}/assets)

    set(valid_asset_types MODEL TEXTURE SOUND)

    unset(current_args)
    foreach( arg IN LISTS ARGN )
        list(FIND valid_asset_types ${arg} asset_type)
        if( NOT asset_type EQUAL -1 )
            if( current_args )
                _td_compile_asset(${target} ${log_file} ${asset_directory} ${current_args})
            endif()
            set(current_args ${arg})
        elseif(current_args)
            list(APPEND current_args ${arg})
        else()
            message(FATAL_ERROR "Missing asset type (MODEL, TEXTURE or SOUND)")
        endif()
    endforeach()

    if( current_args )
        _td_compile_asset(${target} ${log_file} ${asset_directory} ${current_args})
    endif()

endfunction()