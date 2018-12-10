function (minko_enable_plugin_offscreen target)
    set (OFFSCREEN_PATH "${MINKO_HOME}/plugin/offscreen")

    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_OFFSCREEN")
    minko_plugin_link ("offscreen" ${target})
    find_library (OSMESA_LIB NAMES "OSMesa")
    target_link_libraries(${target} ${OSMESA_LIB})
    
    # remove linked libs
    get_property (LLIBS_LIST TARGET ${target} PROPERTY LINK_LIBRARIES)

    list (REMOVE_ITEM LLIBS_LIST "-lGL")
    list (REMOVE_ITEM LLIBS_LIST "-lOpenGL32")
    list (REMOVE_ITEM LLIBS_LIST "-lglew32")
    set_property (TARGET ${target} PROPERTY LINK_LIBRARIES ${LLIBS_LIST})

    file (GLOB
        OFFSCREEN_INCLUDE
        "${OFFSCREEN_PATH}/include"
        "${OFFSCREEN_PATH}/lib/osmesa/linux/include"
    )

    target_include_directories(${target} PRIVATE ${OFFSCREEN_INCLUDE})

    if (WIN32 AND BITNESS EQUAL 32)
        # libdirs { minko.plugin.path("offscreen") .. "/lib/osmesa/windows/lib/x86" }
        get_property (INCLUDE_LIST TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
        list(REMOVE_ITEM INCLUDE_LIST "${MINKO_HOME}/framework/lib/glew/include")
        set_property (TARGET ${target} PROPERTY INCLUDE_DIRECTORIES ${INCLUDE_LIST})
        
        file (GLOB FILES "${OFFSCREEN_PATH}/lib/osmesa/windows/lib/x86/*.dll")
        file (COPY ${FILES} DESTINATION ${OUTPUT_PATH})
    endif ()

    if (WIN32 AND BITNESS EQUAL 64)
        # libdirs { minko.plugin.path("offscreen") .. "/lib/osmesa/windows/lib/x64" }
        get_property (INCLUDE_LIST TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
        list(REMOVE_ITEM INCLUDE_LIST "${MINKO_HOME}/framework/lib/glew/include")
        set_property (TARGET ${target} PROPERTY INCLUDE_DIRECTORIES ${INCLUDE_LIST})

        file (GLOB FILES "${OFFSCREEN_PATH}/lib/osmesa/windows/lib/x64/*.dll")
        file (COPY ${FILES} DESTINATION ${OUTPUT_PATH})
    endif ()
endfunction ()