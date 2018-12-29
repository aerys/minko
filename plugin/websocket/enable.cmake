function (minko_enable_plugin_websocket target)
    set (WEBSOCKET_PATH "${MINKO_HOME}/plugin/websocket")
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