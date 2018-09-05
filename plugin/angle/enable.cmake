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
    file (COPY ${ANGLE_PATH}/lib/win/ANGLE/lib/*.dll ${OUTPUT_PATH})
endfunction ()
