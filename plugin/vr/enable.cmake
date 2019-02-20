function (minko_enable_plugin_vr target)
    set (VR_PATH "${MINKO_HOME}/plugin/vr")
    target_compile_options (${target} PRIVATE "-DMINKO_PLUGIN_VR")

    minko_plugin_link ("vr" ${target})

    get_target_property(TARGET_TYPE ${target} TYPE)
    get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)

    file (GLOB VR_INCLUDE "${VR_PATH}/include")
    target_include_directories(${target} PRIVATE ${VR_INCLUDE})

    if (TARGET_TYPE STREQUAL "EXECUTABLE")
        file (COPY ${VR_PATH}/asset DESTINATION ${OUTPUT_PATH}/asset)
    endif ()

    # minko.package.assetdirs { path.join(minko.plugin.path("vr"), "asset") }
    minko_package_assets(
        ${target}
        COPY
        "${VR_PATH}/asset/effect/*/*.effect"
        "${VR_PATH}/asset/effect/*/*.glsl"
        "${VR_PATH}/asset/script/*.js"
    )

    if (ANDROID OR IOS OR EMSCRIPTEN)
        minko_enable_plugin_sensors (${target})
    endif ()

    if (WIN32)
        find_library (VR_LIBS 
            NAMES
            "winmm"
            "setupapi"
            "ws2_32"
        )
        target_link_libraries(${target} ${VR_LIBS})
    endif ()

    if (LINUX)
        find_library (VR_LIBS
            NAMES
            "udev"
            "Xinerama"
            "X11"
            "pthread"
            "rt"
            "Xrandr"
        )
        target_link_libraries(${target} ${VR_LIBS})
    endif ()

    if (EMSCRIPTEN)
        package_assets (".js" "on")
    endif ()
endfunction ()

# newoption { trigger = "with-vr" }