function (minko_enable_plugin_offscreen target)
    set (OFFSCREEN_PATH "${MINKO_HOME}/plugin/offscreen")

    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_OFFSCREEN")
    minko_plugin_link ("offscreen" ${target})

    # remove linked libs
    get_property (LLIBS_LIST TARGET ${target} PROPERTY LINK_LIBRARIES)
    list (REMOVE_ITEM LLIBS_LIST "-lGL")
    list (REMOVE_ITEM LLIBS_LIST "-lOpenGL32")
    list (REMOVE_ITEM LLIBS_LIST "-lglew32")
    list (REMOVE_ITEM LLIBS_LIST "GL")
    list (REMOVE_ITEM LLIBS_LIST "OpenGL32")
    list (REMOVE_ITEM LLIBS_LIST "glew32")
    set_property (TARGET ${target} PROPERTY LINK_LIBRARIES ${LLIBS_LIST})

    file (GLOB OFFSCREEN_INCLUDE "${OFFSCREEN_PATH}/include")
    target_include_directories(${target} PRIVATE ${OFFSCREEN_INCLUDE})

    if (WIN32)
        file (GLOB OSMESA_INCLUDE "${OFFSCREEN_PATH}/lib/osmesa/windows/include")
        target_include_directories(${target} PRIVATE ${OSMESA_INCLUDE})
    endif ()

    get_target_property(TARGET_TYPE ${target} TYPE)
    if (TARGET_TYPE STREQUAL "EXECUTABLE")
        get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)

        get_property (INCLUDE_LIST TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
        list(REMOVE_ITEM INCLUDE_LIST "${MINKO_HOME}/framework/lib/glew/include")
        set_property (TARGET ${target} PROPERTY INCLUDE_DIRECTORIES ${INCLUDE_LIST})

        if (WIN32)
            if (BITNESS EQUAL 32)
                find_library (
                    OSMESA_LIB
                    NAMES "OSMesa"
                    HINTS "${MINKO_HOME}/plugin/offscreen/lib/osmesa/windows/lib/x86"
                )
                target_link_libraries (${target} ${OSMESA_LIB})
                file (GLOB FILES "${OFFSCREEN_PATH}/lib/osmesa/windows/lib/x86/*.dll")
                file (COPY ${FILES} DESTINATION ${OUTPUT_PATH})
            elseif (BITNESS EQUAL 64)
                find_library (
                    OSMESA_LIB
                    NAMES "OSMesa"
                    HINTS "${MINKO_HOME}/plugin/offscreen/lib/osmesa/windows/lib/x64"
                )
                target_link_libraries (${target} ${OSMESA_LIB})
                file (GLOB FILES "${OFFSCREEN_PATH}/lib/osmesa/windows/lib/x64/*.dll")
                file (COPY ${FILES} DESTINATION ${OUTPUT_PATH})
            endif ()
        else ()
            find_library (OSMESA_LIB NAMES "OSMesa")
            if (OSMESA_LIB)
                target_link_libraries (${target} ${OSMESA_LIB})
            else ()
                message (FATAL_ERROR "Library OSMesa not found.")
            endif ()
        endif ()
    endif ()
endfunction ()

