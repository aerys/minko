function (minko_enable_plugin_http_worker target)
    set (HTTP-WORKER_PATH "${MINKO_HOME}/plugin/http-worker")
    target_include_directories (${target} PRIVATE "${HTTP-WORKER_PATH}/include")
    target_compile_options (${target} PRIVATE -DMINKO_PLUGIN_HTTP_WORKER -DCURL_STATICLIB)

    get_target_property(TARGET_TYPE ${target} TYPE)
    get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)

    if (NOT EMSCRIPTEN)
        minko_plugin_link ("http-worker" ${target})
        # The libcurl for Windows uses the Schannel SSL implementation.
        if (NOT WIN32)
            minko_enable_plugin_ssl(${target})
        endif ()
    endif ()

    minko_set_bitness()
    if (ANDROID)
        target_link_libraries (
            ${target}
            "${HTTP-WORKER_PATH}/lib/curl/android/armeabi-v7a/r25b/lib/libcurl.a"
        )
        minko_plugin_link ("zlib" ${target})
    elseif (LINUX AND BITNESS EQUAL 64)
        target_link_libraries(
            ${target}
            "${HTTP-WORKER_PATH}/lib/curl/linux/amd64/gcc-9.4/lib/libcurl.a"
        )
    elseif (WIN32 AND BITNESS EQUAL 64)
        target_link_libraries(
            ${target}
            "${HTTP-WORKER_PATH}/lib/curl/windows/amd64/msvc/lib/libcurl.lib"
            Ws2_32 Crypt32 Wldap32 Normaliz
        )
    else ()
        message(ERROR "Platform not supported.")
    endif ()

endfunction ()
