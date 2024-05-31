function (minko_enable_plugin_ssl target)
    set (SSL_PATH "${MINKO_HOME}/plugin/ssl")
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_SSL")

    get_target_property(TARGET_TYPE ${target} TYPE)
    get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)

    minko_set_bitness()
    if (ANDROID)
        set (PLATFORM_NAME "android/${ANDROID_ABI}/r25b")
    elseif (LINUX AND BITNESS EQUAL 64)
        set (PLATFORM_NAME "linux/amd64/gcc-9.4")
    elseif (WIN32 AND BITNESS EQUAL 64)
        set (PLATFORM_NAME "windows/amd64/msvc")
    else ()
        message(ERROR "Platform not supported.")
    endif ()

    target_include_directories(${target}
        PRIVATE
        "${SSL_PATH}/include"
        "${SSL_PATH}/lib/openssl/${PLATFORM_NAME}/include"
    )

    if (NOT EMSCRIPTEN AND NOT WIN32)
        minko_plugin_link ("ssl" ${target})
    endif ()

    if (WIN32)
        target_link_libraries(${target}
            "${SSL_PATH}/lib/openssl/${PLATFORM_NAME}/lib/libssl_static.lib"
            "${SSL_PATH}/lib/openssl/${PLATFORM_NAME}/lib/libcrypto_static.lib"
        )
    else ()
        target_link_libraries(${target}
            "${SSL_PATH}/lib/openssl/${PLATFORM_NAME}/lib/libssl.a"
            "${SSL_PATH}/lib/openssl/${PLATFORM_NAME}/lib/libcrypto.a"
        )
endif ()
endfunction ()
