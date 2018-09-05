function (minko_enable_plugin_devil target)
    set (DEVIL_PATH "${MINKO_HOME}/plugin/devil")
    minko_plugin_link ("devil" ${target})
    target_include_directories(${target} PRIVATE "${DEVIL_PATH}/include")
    target_compile_options(${target} PRIVATE -DMINKO_PLUGIN_DEVIL)

    minko_enable_plugin_zlib (${target})
endfunction ()