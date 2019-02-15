function (minko_enable_plugin_ssl target)
    set (SSL_PATH "${MINKO_HOME}/plugin/ssl")
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_SSL")

    target_include_directories(${target} 
        PRIVATE 
        "${SSL_PATH}/include"
        "${SSL_PATH}/lib/openssl/include"
    )
    
    if (NOT EMSCRIPTEN AND NOT WIN32)
        minko_plugin_link ("ssl" ${target})
    endif ()
    
    if (LINUX)
        # find_package(OpenSSL REQUIRED)
        target_link_libraries (${target} "ssl" "crypto")
        
        # if (OPENSSL_FOUND)
        #     message ("Required package OpenSSL has been found. Version: ${OPENSSL_VERSION}")
        #     include_directories(${OPENSSL_INCLUDE_DIRS})
        #     link_directories(${OPENSSL_LIBRARIES})
        # endif ()
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
        file (COPY "${SSL_PATH}/lib/openssl/lib/android/libcrypto.so" DESTINATION "${OUTPUT_PATH}")
        file (COPY "${SSL_PATH}/lib/openssl/lib/android/libssl.so" DESTINATION "${OUTPUT_PATH}")
    endif ()
    if (APPLE AND NOT IOS)
        target_link_libraries(${target}
            "ssl"
            "crypto"
        )
    endif ()
    if (WIN32)
        target_compile_options(
            ${target} PRIVATE
            -DOPENSSL_SYSNAME_WIN32
        )
        target_link_libraries(${target}
            "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/libcrypto.lib"
            "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/libssl.lib"
        )
        if (BITNESS EQUAL 32)
            file (COPY "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/libcrypto-1_1.dll" DESTINATION "${OUTPUT_PATH}")
            file (COPY "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/libssl-1_1.dll" DESTINATION "${OUTPUT_PATH}")
        else ()
            file (COPY "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/libcrypto-1_1-x64.dll" DESTINATION "${OUTPUT_PATH}")
            file (COPY "${SSL_PATH}/lib/openssl/lib/windows${BITNESS}/libssl-1_1-x64.dll" DESTINATION "${OUTPUT_PATH}")
        endif ()
    endif()
endfunction ()