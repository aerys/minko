function (enable_zlib target)
    set (ZLIB_PATH "${MINKO_HOME}/plugin/zlib")
    
    plugin_link ("zlib" ${target})
    
    file (GLOB ${ZLIB_INCLUDE} "${ZLIB_PATH}/lib/zlib")
    target_include_directories(${target} PUBLIC ${ZLIB_INCLUDE})
    target_compile_options (${target} PUBLIC
        -DMINKO_PLUGIN_ZLIB
    )
endfunction ()