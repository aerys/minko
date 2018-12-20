function (minko_enable_plugin_html_overlay target)
    set (HTML_OVERLAY_PATH "${MINKO_HOME}/plugin/html-overlay")

    minko_enable_plugin_jpeg (${target})
    minko_enable_plugin_png (${target})
    minko_enable_plugin_ttf (${target})

    minko_plugin_link ("html-overlay" ${target})
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_HTML_OVERLAY")

    file (GLOB OVERLAY_INCLUDE "${HTML_OVERLAY_PATH}/include")
    target_include_directories (${target} PRIVATE ${OVERLAY_INCLUDE})

    if (EMSCRIPTEN)
        file (COPY ${HTML_OVERLAY_PATH}/asset DESTINATION ${OUTPUT_PATH})
    endif ()

    if (APPLE)
        target_compile_options (${target} PRIVATE -x objective-c++)
    endif ()

    if (IOS)
        file (COPY ${HTML_OVERLAY_PATH}/asset DESTINATION ${OUTPUT_PATH})
        file (COPY ${HTML_OVERLAY_PATH}/lib/WebViewJavascriptBridge/WebViewJavascriptBridge.js.txt DESTINATION ${OUTPUT_PATH})
    endif ()

    if (ANDROID)
        target_link_libraries (
            ${target}
            "-Wl,--undefined=Java_minko_plugin_htmloverlay_InitWebViewTask_webViewInitialized"
            "-Wl,--undefined=Java_minko_plugin_htmloverlay_MinkoWebViewClient_webViewPageLoaded"
            "-Wl,--undefined=Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnMessage"
            "-Wl,--undefined=Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnEvent"
        )
        file (COPY ${HTML_OVERLAY_PATH}/asset DESTINATION ${OUTPUT_PATH})
    endif ()

    if (APPLE AND NOT IOS)
        target_link_libraries(${target} "-framework WebKit")
        file (COPY ${HTML_OVERLAY_PATH}/asset DESTINATION ${OUTPUT_PATH})
        file (COPY ${HTML_OVERLAY_PATH}/lib/WebViewJavascriptBridge/WebViewJavascriptBridge.js.txt DESTINATION ${OUTPUT_PATH})
    endif ()

    if (WIN32)
        find_library (RETURN_PATH NAMES "libcef")
        target_link_libraries(${target} RETURN_PATH)
        file (COPY ${HTML_OVERLAY_PATH}/asset DESTINATION ${OUTPUT_PATH})
        
        file (
            GLOB
            WIN32_FILES
            "${HTML_OVERLAY_PATH}/lib/resource/*"
            "${HTML_OVERLAY_PATH}/lib/win/dll/*.dll"
        )
        
        file (COPY ${HTML_OVERLAY_PATH}/asset DESTINATION ${OUTPUT_PATH})
        file (COPY ${WIN32_FILES} DESTINATION ${OUTPUT_PATH})
    endif ()

    # libdirs { minko.plugin.path("html-overlay") .. "/lib/win/debug" }

    if (LINUX)
        target_compile_options (${target} PRIVATE "-Wl,-rpath,.")
        find_library (RETURN_PATH NAMES "cef")
        target_link_libraries (${target} RETURN_PATH)

        file (COPY ${HTML_OVERLAY_PATH}/asset DESTINATION ${OUTPUT_PATH})
        minko_copy (${HTML_OVERLAY_PATH}/lib/resource/locales ${OUTPUT_PATH} ${target})
        file (COPY ${HTML_OVERLAY_PATH}/lib/resource/cef.pak DESTINATION ${OUTPUT_PATH})
        file (COPY ${HTML_OVERLAY_PATH}/lib/resource/devtools_resources.pak DESTINATION ${OUTPUT_PATH})

        # libdirs { minko.plugin.path("html-overlay") .. "/lib/linux32/" }
        # or libdirs { minko.plugin.path("html-overlay") .. "/lib/linux64/" }
        minko_copy (${HTML_OVERLAY_PATH}/lib/linux64/chrome-sandbox ${OUTPUT_PATH} ${target})
        file (COPY ${HTML_OVERLAY_PATH}/lib/linux64/libcef.so DESTINATION ${OUTPUT_PATH})
        file (COPY ${HTML_OVERLAY_PATH}/lib/linux64/libffmpegsumo.so DESTINATION ${OUTPUT_PATH})
    endif ()
endfunction()