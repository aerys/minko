function (minko_enable_plugin_devil target)
    set (DEVIL_PATH "${MINKO_HOME}/plugin/devil")
    minko_plugin_link ("devil" ${target})
    target_include_directories(${target} PRIVATE "${DEVIL_PATH}/include")
    target_compile_options(${target} PRIVATE -DMINKO_PLUGIN_DEVIL)

    get_target_property(TARGET_TYPE ${target} TYPE)
    get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)

    if (TARGET_TYPE STREQUAL "EXECUTABLE" AND (LINUX OR WIN32) AND BITNESS EQUAL 64)
        if (WIN32)
            set (DEVIL_LIBS "${MINKO_HOME}/plugin/devil/lib/devil/DevIL/lib/x64/*.lib")
            set (DEVIL_SHARED_LIBS "${MINKO_HOME}/plugin/devil/lib/devil/DevIL/lib/x64/*.dll")
        endif ()
        if (LINUX)
            set (DEVIL_LIBS "${MINKO_HOME}/plugin/devil/lib/devil/DevIL/lib/x64/*.so")
            set (DEVIL_SHARED_LIBS "${MINKO_HOME}/plugin/devil/lib/devil/DevIL/lib/x64/*.so")
        endif ()
        file (GLOB DEVIL_LIBS ${DEVIL_LIBS})
        file (GLOB DEVIL_SHARED_LIBS ${DEVIL_SHARED_LIBS})
        file (COPY ${DEVIL_SHARED_LIBS} DESTINATION ${OUTPUT_PATH})
        target_link_libraries(
            ${target}
            ${DEVIL_LIBS}
        )
    endif ()

    # Enabling zlib by default my cause multiple definition linking errors.
    # So the app CMakeLists.txt must enable the zlib plugin on a case-by-case basis.
    # minko_enable_plugin_zlib (${target})
endfunction ()