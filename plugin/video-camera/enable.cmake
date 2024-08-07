function (minko_enable_plugin_video_camera target)
    set (VIDEO_CAMERA_PATH "${MINKO_HOME}/plugin/video-camera")

    get_target_property(TARGET_TYPE ${target} TYPE)
    get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)
    
    minko_plugin_link ("video-camera" ${target})
    file (GLOB
        VIDEO_CAMERA_INCLUDE
        "${VIDEO_CAMERA_PATH}/include"
    )
    target_include_directories(${target} PRIVATE ${VIDEO_CAMERA_INCLUDE})
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_VIDEO_CAMERA")

    if (EMSCRIPTEN and TARGET_TYPE STREQUAL "EXECUTABLE")
        file (COPY ${VIDEO_CAMERA_PATH}/asset DESTINATION ${OUTPUT_PATH}/asset)
    endif ()

    if (IOS)
        target_compile_options (${target} PRIVATE -x objective-c++)
        target_link_libraries (${target}
            "-framework AVFoundation"
            "-framework CoreVideo"
            "-framework CoreMedia"
        )
    endif ()
endfunction ()
