function (minko_enable_plugin_websocket target)
    set (WEBSOCKET_PATH "${MINKO_HOME}/plugin/websocket")

    if (ANDROID)
        # libnode.so exports its own OpenSSL: an archive linked after it would send its OpenSSL
        # calls there instead of to the static OpenSSL of the ssl plugin.
        get_target_property (DEFINITIONS ${target} COMPILE_DEFINITIONS)
        list (FIND DEFINITIONS "MINKO_PLUGIN_NODEJS_WORKER" INDEX)
        if (NOT INDEX EQUAL -1)
            message (FATAL_ERROR "Enable the websocket plugin before the nodejs-worker plugin: libnode.so exports its own OpenSSL.")
        endif ()
    endif ()

    minko_plugin_link ("websocket" ${target})

    if (NOT EMSCRIPTEN)
        minko_enable_plugin_ssl (${target})
        
        file (GLOB
            WEBSOCKET_INCLUDE
            "${WEBSOCKET_PATH}/lib/websocketpp"
            "${WEBSOCKET_PATH}/lib/asio/include"
        )
        target_include_directories(${target} PRIVATE ${WEBSOCKET_INCLUDE})
        target_compile_options (${target} PRIVATE "-DASIO_STANDALONE")
    endif ()

    if (WIN32)
        target_compile_options (${target} PRIVATE
            "-D_WEBSOCKETPP_CPP11_INTERNAL_"
            "-D_WIN32_WINNT=0x0501"
        )
    endif ()

    file (GLOB WEBSOCKET_INCLUDE "${WEBSOCKET_PATH}/include")
    target_include_directories(${target} PRIVATE ${WEBSOCKET_INCLUDE})
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_WEBSOCKET")

    if (LINUX OR ANDROID OR APPLE)
        target_compile_options (${PROJECT_NAME} PRIVATE
            "-DSSL_R_SHORT_READ=SSL_R_UNEXPECTED_RECORD"
        )
    endif ()
endfunction ()