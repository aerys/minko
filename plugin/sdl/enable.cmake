function (minko_enable_plugin_sdl target)
    minko_plugin_link("sdl" ${target})

    # add requested offscreen
    if (WITH_OFFSCREEN STREQUAL "ON" OR WITH_OFFSCREEN STREQUAL "ON")
        minko_enable_plugin_offscreen (${target})
    endif ()

    set(SDL_PATH "${MINKO_HOME}/plugin/sdl")
    list (APPEND SDL_INCLUDE "${SDL_PATH}/include")
    target_compile_options(${target} PRIVATE "-DMINKO_PLUGIN_SDL")

    if (EMSCRIPTEN)
        target_include_directories(${target} PRIVATE ${SDL_INCLUDE})
        target_compile_options (${target} PRIVATE "-sUSE_SDL")
        return ()
    endif ()

    minko_set_bitness()
    if (ANDROID)
        set (PLATFORM_NAME "android/${ANDROID_ABI}/r25b")
    elseif (LINUX AND BITNESS EQUAL 64)
        set (PLATFORM_NAME "linux/amd64/gcc-9.4")
    elseif (WIN32 AND BITNESS EQUAL 64)
        set (PLATFORM_NAME "windows/amd64/msvc")
    else ()
        message(ERROR "Platform not supported.")
    endif ()

    list (APPEND SDL_INCLUDE "${SDL_PATH}/lib/sdl/${PLATFORM_NAME}/include")
    list (APPEND SDL_INCLUDE "${SDL_PATH}/lib/sdl_mixer/${PLATFORM_NAME}/include")
    link_directories("${SDL_PATH}/lib/sdl/${PLATFORM_NAME}/lib")
    link_directories("${SDL_PATH}/lib/sdl_mixer/${PLATFORM_NAME}/lib")

    if (ANDROID)
        target_link_libraries(${target}
            "${SDL_PATH}/lib/sdl_mixer/${PLATFORM_NAME}/lib/libSDL2_mixer.a"
            "${SDL_PATH}/lib/sdl/${PLATFORM_NAME}/lib/libSDL2.a"
            "-lm -lOpenSLES -ldl -llog -landroid -lGLESv1_CM -lGLESv2"
        )
        # Also make sure the SDL .java files are in
        # smartshape-engine/template/android/src/org/libsdl/app/
    elseif (LINUX AND BITNESS EQUAL 64)
        target_link_libraries(${target}
            "${SDL_PATH}/lib/sdl_mixer/${PLATFORM_NAME}/lib/libSDL2_mixer.a"
            "${SDL_PATH}/lib/sdl/${PLATFORM_NAME}/lib/libSDL2.a"
            ${CMAKE_DL_LIBS}
        )
    elseif (WIN32 AND BITNESS EQUAL 64)
        target_link_libraries(${target}
            "${SDL_PATH}/lib/sdl_mixer/${PLATFORM_NAME}/lib/SDL2_mixer-static.lib"
            "${SDL_PATH}/lib/sdl/${PLATFORM_NAME}/lib/SDL2-static.lib"
            ${CMAKE_DL_LIBS}
            user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32 dinput8
        )
    else ()
        message(ERROR "Platform not supported.")
    endif ()

    target_include_directories(${target}
        PRIVATE
        ${SDL_INCLUDE}
    )
endfunction ()
