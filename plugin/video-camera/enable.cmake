function (minko_enable_plugin_video_camera target)
    set (VIDEO_CAMERA_PATH "${MINKO_HOME}/plugin/video-camera")
    
    minko_plugin_link ("video-camera" ${target})
    file (GLOB
        VIDEO_CAMERA_INCLUDE
        "${VIDEO_CAMERA_PATH}/include"
    )
    target_include_directories(${target} PRIVATE ${VIDEO_CAMERA_INCLUDE})
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_VIDEO_CAMERA")

    if (EMSCIPTEN)
        minko_copy (${VIDEO_CAMERA_PATH}/asset ${OUTPUT_PATH} ${target})
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
