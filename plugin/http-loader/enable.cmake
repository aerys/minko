function (minko_enable_plugin_http_loader target)
    set (HTTP-LOADER_PATH "${MINKO_HOME}/plugin/http-loader")
    minko_plugin_link ("http-loader" ${target})
    target_include_directories(${target} PRIVATE "${HTTP-LOADER_PATH}/include")
    target_compile_options(${target} PRIVATE -DMINKO_PLUGIN_HTTP_LOADER)
    minko_enable_plugin_http_worker (${target})
endfunction ()