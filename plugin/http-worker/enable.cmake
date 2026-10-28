function (minko_enable_plugin_http_worker target)
    set (HTTP-WORKER_PATH "${MINKO_HOME}/plugin/http-worker")
    target_include_directories (${target} PRIVATE "${HTTP-WORKER_PATH}/include")
    target_compile_options (${target} PRIVATE -DMINKO_PLUGIN_HTTP_WORKER -DCURL_STATICLIB)

    get_target_property(TARGET_TYPE ${target} TYPE)
    get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)

    if (ANDROID)
        # libnode.so exports its own OpenSSL: an archive linked after it would send its OpenSSL
        # calls there instead of to the static OpenSSL of the ssl plugin.
        get_target_property (DEFINITIONS ${target} COMPILE_DEFINITIONS)
        list (FIND DEFINITIONS "MINKO_PLUGIN_NODEJS_WORKER" INDEX)
        if (NOT INDEX EQUAL -1)
            message (FATAL_ERROR "Enable the http-worker plugin before the nodejs-worker plugin: libnode.so exports its own OpenSSL.")
        endif ()
    endif ()

    if (NOT EMSCRIPTEN)
        minko_plugin_link ("http-worker" ${target})
        # The libcurl for Windows uses the Schannel SSL implementation.
        if (NOT WIN32)
            minko_enable_plugin_ssl(${target})
        endif ()
    endif ()

    minko_set_bitness()
    if (ANDROID)
        # Built with the NDK of the engine (see cmake/minko.cmake).
        if (NOT MINKO_ANDROID_NDK_MAJOR)
            message (FATAL_ERROR "MINKO_ANDROID_NDK_MAJOR is not set: include cmake/minko.cmake (or cmake/sdk.cmake) once the Android toolchain is loaded.")
        endif ()
        target_link_libraries (
            ${target}
            "${HTTP-WORKER_PATH}/lib/curl/android/${ANDROID_ABI}/r${MINKO_ANDROID_NDK_MAJOR}/lib/libcurl.a"
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
    elseif (NOT EMSCRIPTEN)
        message(FATAL_ERROR "Platform not supported.")
    endif ()

endfunction ()
