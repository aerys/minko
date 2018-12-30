function (minko_enable_plugin_http_worker target)
    set (HTTP-WORKER_PATH "${MINKO_HOME}/plugin/http-worker")
    target_include_directories (${target} PRIVATE "${HTTP-WORKER_PATH}/include")
    target_compile_options (${target} PRIVATE -DMINKO_PLUGIN_HTTP_WORKER -DCURL_STATICLIB)

    if (NOT EMSCRIPTEN)
        minko_plugin_link ("http-worker" ${target})
        minko_enable_plugin_ssl(${target})
    endif ()

    if (WIN32)
        if (BITNESS EQUAL 32)
            target_link_libraries(${target} "${HTTP-WORKER_PATH}/lib/curl/lib/windows${BITNESS}/libcurl.lib")
            file (COPY "${HTTP-WORKER_PATH}/lib/curl/lib/windows${BITNESS}/libcurl.dll" DESTINATION ${OUTPUT_PATH})
        else ()
            target_link_libraries(${target} "${HTTP-WORKER_PATH}/lib/curl/lib/windows${BITNESS}/libcurl-x64.lib")
            file (COPY "${HTTP-WORKER_PATH}/lib/curl/lib/windows${BITNESS}/libcurl-x64.dll" DESTINATION ${OUTPUT_PATH})
        endif ()
        
    endif ()

    if (LINUX)
        target_link_libraries (${target} "curl")
    endif ()
    
    if (APPLE AND NOT IOS)
        target_link_libraries (
            ${target}
            "${HTTP-WORKER_PATH}/lib/curl/lib/osx64/release/libcurl.a"
            "-framework Security"
            "-framework LDAP"
        )
        minko_plugin_link ("zlib" ${target})
    endif ()

    if (IOS)
        target_link_libraries (${target} "${HTTP-WORKER_PATH}/lib/curl/lib/ios/libcurl.a" "-framework Security")
        minko_plugin_link ("zlib" ${target})
    endif ()

    if (ANDROID)
        target_link_libraries (${target} "${HTTP-WORKER_PATH}/lib/curl/lib/android/libcurl.a")
        minko_plugin_link ("zlib" ${target})
    endif ()
endfunction ()