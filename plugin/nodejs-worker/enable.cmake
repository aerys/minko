function (minko_enable_plugin_nodejs_worker target)
    set (NODEJS-WORKER_PATH "${MINKO_HOME}/plugin/nodejs-worker")
    target_include_directories (
        ${target}
        PRIVATE
        "${NODEJS-WORKER_PATH}/include"
    )
    message("${NODEJS-WORKER_PATH}/include")
    target_compile_definitions (
        ${target}
        PRIVATE
        "MINKO_PLUGIN_NODEJS_WORKER"
    )
    if (NOT EMSCRIPTEN)
        minko_enable_plugin_ssl (${target})
    endif ()
    if (ANDROID)
        minko_plugin_link ("nodejs-worker" ${target})
        set (NODE_LIB "${NODEJS-WORKER_PATH}/lib/nodejs/android/libnode.so")
        file (
            COPY ${NODE_LIB}
            DESTINATION ${OUTPUT_PATH}
        )
        target_link_libraries(${target} "${NODEJS-WORKER_PATH}/lib/nodejs/android/libnode.so")
    endif ()
endfunction ()