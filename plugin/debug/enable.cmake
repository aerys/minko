function (minko_enable_plugin_debug target)
    set (DEBUG_PATH "${MINKO_HOME}/plugin/debug")
    minko_plugin_link ("debug" ${target})

    file (
        GLOB
        ${PROJECT_NAME}_INCLUDE_DEBUG
        "${DEBUG_PATH}/include"
    )

    target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE_DEBUG})
    target_compile_options (${PROJECT_NAME} PRIVATE -DMINKO_PLUGIN_DEBUG)
    
    file (COPY ${DEBUG_PATH}/asset DESTINATION ${OUTPUT_PATH}/asset)
endfunction ()