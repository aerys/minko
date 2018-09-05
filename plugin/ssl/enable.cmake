function (minko_enable_plugin_ssl target)
    set (SSL_PATH "${MINKO_HOME}/plugin/ssl")
    target_include_directories(${target} 
        PRIVATE 
        "${SSL_PATH}/include"
        "${SSL_PATH}/lib/openssl/include"
    )
    #[[ if (NOT EMSCRIPTEN)
        minko_plugin_link ("ssl" ${target})
    endif () ]]
    if (LINUX)
        target_link_libraries(${target}
            "ssl"
            "crypto"
        )
    endif ()
    if (IOS)
        target_link_libraries(${target}
            "${SSL_PATH}/lib/openssl/lib/ios/libssl.a"
            "${SSL_PATH}/lib/openssl/lib/ios/libcrypto.a"
        )
    endif ()
    if (ANDROID)
        target_link_libraries(${target}
            "${SSL_PATH}/lib/openssl/lib/android/libssl.a"
            "${SSL_PATH}/lib/openssl/lib/android/libcrypto.a"
        )
    endif ()
    if (APPLE AND NOT IOS)
        target_link_libraries(${target}
            "ssl"
            "crypto"
        )
    endif ()
    if (WIN32)
        target_compile_options(${target}
            PRIVATE
            -DOPENSSL_SYSNAME_WIN32
        )
        target_link_libraries(${target}
            "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/libeay32.lib"
            "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/ssleay32.lib"
        )
        file (COPY "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/ssleay32.dll" DESTINATION "${OUTPUT_PATH}")
        file (COPY "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/libeay32.dll" DESTINATION "${OUTPUT_PATH}")
    endif()
endfunction ()