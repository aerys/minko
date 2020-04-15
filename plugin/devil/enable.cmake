function (minko_enable_plugin_devil target)
    set (DEVIL_PATH "${MINKO_HOME}/plugin/devil")
    minko_plugin_link ("devil" ${target})
    target_include_directories(${target} PRIVATE "${DEVIL_PATH}/include")
    target_compile_options(${target} PRIVATE -DMINKO_PLUGIN_DEVIL)

    get_target_property(TARGET_TYPE ${target} TYPE)
    get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)

    if ((LINUX OR WIN32) AND BITNESS EQUAL 64)
        if (WIN32)
            set (DEVIL_SHARED_LIBRARIES_LOCATION "${MINKO_HOME}/plugin/devil/lib/devil/DevIL/lib/x64/*.dll")
        endif ()
        if (LINUX)
            set (DEVIL_SHARED_LIBRARIES_LOCATION "${MINKO_HOME}/plugin/devil/lib/devil/DevIL/lib/x64/*.so")
        endif ()
        if (TARGET_TYPE STREQUAL "EXECUTABLE")
            file (GLOB DEVIL_SHARED_LIBRARIES ${DEVIL_SHARED_LIBRARIES_LOCATION})
            file (COPY ${DEVIL_SHARED_LIBRARIES} DESTINATION ${OUTPUT_PATH})
        endif ()
    endif ()

    target_link_libraries(
        ${target}
        ${DEVIL_SHARED_LIBRARIES}
    )

    # Enabling zlib by default my cause multiple definition linking errors.
    # So the app CMakeLists.txt must enable the zlib plugin on a case-by-case basis.
    # minko_enable_plugin_zlib (${target})
endfunction ()