function (enable_zlib target)
    set (ZLIB_PATH "${MINKO_HOME}/plugin/zlib")
    
    plugin_link ("zlib" ${PROJECT_NAME})
    
    file (GLOB ${PROJECT_NAME}_INCLUDE "${ZLIB_PATH}/lib/zlib")

    target_compile_options (${PROJECT_NAME} PUBLIC
        -DMINKO_PLUGIN_ZLIB
    )

    target_link_libraries (${target} "z")
endfunction ()