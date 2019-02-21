function (minko_enable_plugin_angle target)
    # if (WIN32) + console stuff
    set (ANGLE_PATH "${MINKO_HOME}/plugin/angle")
    target_compile_options (${target} PRIVATE -DMINKO_PLUGIN_ANGLE)

    find_library (
        EGL_LIB
        NAMES "libEGL" "libGLESv2"
        HINTS "${SERIALIZER_PATH}/lib/win/ANGLE/lib"
    )
    target_link_libraries (${target} EGL_LIB)

    get_target_property(TARGET_TYPE ${target} TYPE)
    if (TARGET_TYPE STREQUAL "EXECUTABLE")
        get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)
        file (COPY ${ANGLE_PATH}/lib/win/ANGLE/lib/*.dll ${OUTPUT_PATH})
    endif ()

endfunction ()
