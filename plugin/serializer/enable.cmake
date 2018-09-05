function (minko_enable_plugin_serializer target)
    set (SERIALIZER_PATH "${MINKO_HOME}/plugin/serializer")
    
    minko_enable_plugin_jpeg (${target})
    minko_enable_plugin_png (${target})
    minko_enable_plugin_ttf (${target})

    minko_plugin_link ("serializer" ${target})
    
    file (GLOB
        SERIALIZER_INCLUDE
        "${SERIALIZER_PATH}/include"
        "${SERIALIZER_PATH}/lib/msgpack-c/include"
    )
    target_include_directories(${target} PRIVATE ${SERIALIZER_INCLUDE})
    target_compile_options (${target} PRIVATE -DMINKO_PLUGIN_SERIALIZER)

    if (WITH_TEXTURE_COMPRESSOR STREQUAL "on" OR WITH_TEXTURE_COMPRESSOR STREQUAL "ON")
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
            target_link_libraries(${target} RET_PATH)
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
            target_link_libraries(${target} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/Windows_x86_32/Dynamic/*.dll ${OUTPUT_PATH})
        endif ()

        if (LINUX AND BITNESS EQUAL 32)
            find_library (RET_PATH
                NAMES
                "PVRTexLib"
                "TextureConverter"
                "QCompressLib"
                HINTS
                "${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic"
                "${SERIALIZER_PATH}/lib/QCompress/Lib/ubuntu/i386"
            )
            target_link_libraries(${target} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic/*.so ${OUTPUT_PATH})
            file (COPY ${SERIALIZER_PATH}/lib/QCompress/Lib/ubuntu/i386/*.so ${OUTPUT_PATH})
        endif ()

        if (LINUX AND BITNESS EQUAL 64)
            find_library (RET_PATH
                NAMES
                "PVRTexLib"
                HINTS
                "${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic"
            )
            target_link_libraries(${target} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/Linux_x86_32/Dynamic/*.so ${OUTPUT_PATH})
        endif ()

        if (LINUX)
            set_target_properties (${target} PROPERTIES LINK_FLAGS "-Wl,-rpath=.")
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
            target_link_libraries(${target} RET_PATH)
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
            target_link_libraries(${target} RET_PATH)
            file (COPY ${SERIALIZER_PATH}/lib/PVRTexTool/OSX_x86/Dynamic/*.dylib ${OUTPUT_PATH})
        endif ()
    endif ()

    # function minko.plugin.serializer:dist(pluginDistDir)
    # newoption { trigger	= "with-texture-compressor" }
endfunction ()