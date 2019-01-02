function (minko_enable_plugin_nodejs_worker target)
    set (NODEJS_WORKER_PATH "${MINKO_HOME}/plugin/nodejs-worker")
    target_include_directories (
        ${target}
        PRIVATE
        "${NODEJS_WORKER_PATH}/include"
    )
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
        set (NODE_LIB "${NODEJS_WORKER_PATH}/lib/nodejs/lib/android/libnode.so")
        file (
            COPY ${NODE_LIB}
            DESTINATION ${OUTPUT_PATH}
        )
        target_link_libraries(${target} "${NODEJS_WORKER_PATH}/lib/nodejs/lib/android/libnode.so")

        if (NOT "${ANDROID_STL}" STREQUAL "c++_shared")
            message(FATAL_ERROR "ANDROID_STL has to be set to c++_shared when enabling the nodejs-worker plugin")
        endif ()
    endif ()
endfunction ()