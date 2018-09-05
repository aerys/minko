function (minko_enable_plugin_zlib target)
    set (ZLIB_PATH "${MINKO_HOME}/plugin/zlib")

    minko_plugin_link ("zlib" ${target})

    target_include_directories(${target} PRIVATE "${ZLIB_PATH}/lib/zlib")
    target_compile_options (${target} PRIVATE
        -DMINKO_PLUGIN_ZLIB
    )
endfunction ()