function (minko_enable_plugin_html_overlay target)
    set (HTML_OVERLAY_PATH "${MINKO_HOME}/plugin/html-overlay")
    
    # Determine the platform.
    set (CEF_PLATFORM "${SYSTEM_NAME}${BITNESS}")
    if (APPLE AND NOT IOS)
        set (CEF_PLATFORM "macosx64")
    endif ()
    
    # Include CEF toolchain
    include(${MINKO_HOME}/DownloadCEF.cmake)

    # Specify the CEF distribution version (in both CMakeLists.txt and enable.cmake).
    set (CEF_VERSION "3.3538.1852.gcb937fc")
    set (THIRD_PARTY_PATH "${MINKO_HOME}/third_party/cef/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}")

    # Download and extract the CEF binary distribution.
    set (CEF_OUTPUT_PATH "${MINKO_HOME}/third_party/cef")
    if (LINUX OR WIN32 OR APPLE AND NOT IOS)
        DownloadCEF("${CEF_PLATFORM}" "${CEF_VERSION}" "${CEF_OUTPUT_PATH}")
    endif ()

    if (LINUX)
        add_custom_command (
            PRE_BUILD
            TARGET ${PROJECT_NAME}
            COMMAND strip ${THIRD_PARTY_PATH}/Release/libcef.so
        )
    endif ()

    minko_enable_plugin_jpeg (${target})
    minko_enable_plugin_png (${target})
    minko_enable_plugin_ttf (${target})

    minko_plugin_link ("html-overlay" ${target})
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_HTML_OVERLAY")

    file (GLOB OVERLAY_INCLUDE "${HTML_OVERLAY_PATH}/include")
    target_include_directories (${target} PRIVATE ${OVERLAY_INCLUDE})

    file (COPY ${HTML_OVERLAY_PATH}/asset DESTINATION ${OUTPUT_PATH})

    if (APPLE)
        target_compile_options (${target} PRIVATE -x objective-c++)
    endif ()

    if (IOS)
        file (COPY ${HTML_OVERLAY_PATH}/lib/WebViewJavascriptBridge/WebViewJavascriptBridge.js.txt DESTINATION ${OUTPUT_PATH})
    endif ()

    if (ANDOID)
        set_target_properties (
            ${target} PROPERTIES_LINK_FLAGS
            "-Wl,--undefined=Java_minko_plugin_htmloverlay_InitWebViewTask_webViewInitialized"
            "-Wl,--undefined=Java_minko_plugin_htmloverlay_MinkoWebViewClient_webViewPageLoaded"
            "-Wl,--undefined=Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnMessage"
            "-Wl,--undefined=Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnEvent"
        )
    endif ()

    if (APPLE AND NOT IOS)
        target_link_libraries(${target} "-framework WebKit")
        file (COPY ${HTML_OVERLAY_PATH}/lib/WebViewJavascriptBridge/WebViewJavascriptBridge.js.txt DESTINATION ${OUTPUT_PATH})
    endif ()

    if (WIN32)
        file (COPY ${THIRD_PARTY_PATH}/Release/libcef.lib DESTINATION ${OUTPUT_PATH})
        target_link_libraries (${target} ${OUTPUT_PATH}/libcef.lib)

        file (GLOB RESOURCES "${THIRD_PARTY_PATH}/Resources/*")
        file (GLOB RELEASE_CONTENT "${THIRD_PARTY_PATH}/Release/*")
        
        file (COPY ${RESOURCES} DESTINATION ${OUTPUT_PATH})
        file (COPY ${RELEASE_CONTENT} DESTINATION ${OUTPUT_PATH})
        
        file (GLOB WIN32_FILES "${THIRD_PARTY_PATH}/lib/win/dll/*.dll")
        file (COPY ${WIN32_FILES} DESTINATION ${OUTPUT_PATH})
    endif ()

    if (LINUX)
        set_target_properties(${target} PROPERTIES LINK_FLAGS "-Wl,-rpath,./")
        
        file (COPY ${THIRD_PARTY_PATH}/Release/libcef.so DESTINATION ${OUTPUT_PATH})
        target_link_libraries (${target} ${OUTPUT_PATH}/libcef.so)
        
        file (GLOB RESOURCES "${THIRD_PARTY_PATH}/Resources/*")
        file (GLOB RELEASE_CONTENT "${THIRD_PARTY_PATH}/Release/*")
        
        file (COPY ${RESOURCES} DESTINATION ${OUTPUT_PATH})
        file (COPY ${RELEASE_CONTENT} DESTINATION ${OUTPUT_PATH})
    endif ()
endfunction()