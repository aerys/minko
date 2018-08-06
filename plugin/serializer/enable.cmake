function (enable_serializer target)
    set (SERIALIZER_PATH "${MINKO_HOME}/plugin/serializer")

    list (APPEND
        ${PROJECT_NAME}_PLUGINS
        jpeg
        png
        ttf
    )

    foreach (${POJECT_NAME}_PLUGIN IN LIST ${PROJECT_NAME}_PLUGINS)
        call_plugin (enable_${${PROJECT_NAME}_PLUGIN} ${${PROJECT_NAME}_PLUGIN} ${PROJECT_NAME})
    endforeach ()

    plugin_link("serializer" ${PROJECT})
    
    file (GLOB
        ${PROJECT_NAME}_INCLUDE
        "${SERIALIZER_PATH}/include"
        "${SERIALIZER_PATH}/lib/msgpack-c/include"
    )

    target_compile_options (${PROJECT_NAME} PUBLIC -DMINKO_PLUGIN_SERIALIZER)

    if (${WITH_TEXTURE_COMPRESSOR} STREQUAL "on")
        if (WIN32 AND BITNESS EQUAL 32)
            find_library (RET_PATH
                NAMES
                "PVRTexLib"
				"QCompressLib"
                "TextureConverter"
                HINTS
                "${SERIALIZER_PATH}/lib/PVRTexTool/Windows_x86_32/Dynamic"
                "${SERIALIZER_PATH}/lib/QCompress/Lib/windows/Win32"
            )
            target_link_libraries(${PROJECT_NAME} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/Windows_x86_32/Dynamic/*.dll ${OUTPUT_PATH})
            file (COPY ${SERIALIZER_PATH}/lib/QCompress/Lib/windows/Win32/*.dll ${OUTPUT_PATH})
        endif ()

        if (WIN32 AND BITNESS EQUAL 64)
            find_library (RET_PATH
                NAMES
                "PVRTexLib"
                HINTS
                "${SERIALIZER_PATH}/lib/PVRTexTool/Windows_x86_64/Dynamic"
            )
            target_link_libraries(${PROJECT_NAME} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/Windows_x86_32/Dynamic/*.dll ${OUTPUT_PATH})
        endif ()

        if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND BITNESS EQUAL 32)
            find_library (RET_PATH
                NAMES
                "PVRTexLib"
				"TextureConverter"
				"QCompressLib"
                HINTS
                "${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic"
                "${SERIALIZER_PATH}/lib/QCompress/Lib/ubuntu/i386"
            )
            target_link_libraries(${PROJECT_NAME} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic/*.so ${OUTPUT_PATH})
            file (COPY ${SERIALIZER_PATH}/lib/QCompress/Lib/ubuntu/i386/*.so ${OUTPUT_PATH})
        endif ()

        if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND BITNESS EQUAL 64)
            find_library (RET_PATH
                NAMES
                "PVRTexLib"
                HINTS
                "${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic"
            )
            target_link_libraries(${PROJECT_NAME} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic/*.so ${OUTPUT_PATH})
        endif ()

        if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
            set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath=.")
        endif ()

        if (APPLE AND NOT IOS AND BITNESS EQUAL 32)
            find_library (RET_PATH
                NAMES
                "PVRTexLib"
                "QCompressLib"
                HINTS
                "${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic"
                "${SERIALIZER_PATH}/lib/QCompress/Lib/osx/x86"
            )
            target_link_libraries(${PROJECT_NAME} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/OSX_x86/Dynamic/*.dylib ${OUTPUT_PATH})
            file (COPY ${SERIALIZER_PATH}/lib/QCompress/Lib/osx/x86/*.dylib ${OUTPUT_PATH})
        endif ()

        if (APPLE AND NOT IOS AND BITNESS EQUAL 64)
            find_library (RET_PATH
                NAMES
                "PVRTexLib"
                HINTS
                "${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic"
            )
            target_link_libraries(${PROJECT_NAME} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/OSX_x86/Dynamic/*.dylib ${OUTPUT_PATH})
        endif ()
    endif ()

    # function minko.plugin.serializer:dist(pluginDistDir)
    # newoption { trigger	= "with-texture-compressor" }
endfunction ()