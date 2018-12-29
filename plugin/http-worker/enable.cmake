function (minko_enable_plugin_http_worker target)
    set (HTTP-WORKER_PATH "${MINKO_HOME}/plugin/http-worker")
    target_include_directories (${target} PRIVATE "${HTTP-WORKER_PATH}/include")
    target_compile_options (${target} PRIVATE -DMINKO_PLUGIN_HTTP_WORKER -DCURL_STATICLIB)

    if (NOT EMSCRIPTEN)
        minko_plugin_link ("http-worker" ${target})
        minko_enable_plugin_ssl(${target})
    endif ()

    if (WIN32)
        target_link_libraries(${target} "${HTTP-WORKER_PATH}/lib/curl/lib/windows${BITNESS}/${BUILD_TYPE}/libcurl.lib")
        
        file (
            GLOB
            HTTP-WORKER_DLL
            "${HTTP-WORKER_PATH}/lib/curl/lib/windows${BITNESS}/${BUILD_TYPE}/*.dll"
        )
        foreach(HTTP-WORKER_A_DLL ${HTTP-WORKER_DLL})
            file (COPY ${HTTP-WORKER_A_DLL} DESTINATION ${OUTPUT_PATH})
        endforeach()
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