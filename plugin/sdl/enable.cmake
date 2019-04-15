function (minko_enable_plugin_sdl target)
    minko_plugin_link("sdl" ${target})
    target_compile_options(${target} PRIVATE "-DMINKO_PLUGIN_SDL")
    set(SDL_PATH "${MINKO_HOME}/plugin/sdl")
    list (APPEND
        SDL_INCLUDE
        "${SDL_PATH}/include"
    )
    if (NOT EMSCRIPTEN)
        list (APPEND
            SDL_INCLUDE
            "${SDL_PATH}/lib/sdl/include"
        )
    else ()
        list (APPEND
            SDL_INCLUDE
            "SDL"
        )
    endif ()
    target_include_directories(${target}
        PRIVATE
        ${SDL_INCLUDE}
    )
    if (WIN32)
        find_library(SDL2_LIB SDL2 HINTS "${SDL_PATH}/lib/sdl/lib/windows${BITNESS}")
        find_library(SDL2_MAIN_LIB SDL2main HINTS "${SDL_PATH}/lib/sdl/lib/windows${BITNESS}")
        find_library(SDL2_MIXER_LIB SDL2_mixer HINTS "${SDL_PATH}/lib/sdl/lib/windows${BITNESS}")
        target_link_libraries(${target}
            ${SDL2_LIB}
            ${SDL2_MAIN_LIB}
            ${SDL2_MIXER_LIB}
        )
        file (GLOB
            WINDOWS_DLL
            "${SDL_PATH}/lib/sdl/lib/windows${BITNESS}/*.dll"
        )
        get_target_property(TARGET_TYPE ${target} TYPE)
        if (TARGET_TYPE STREQUAL "EXECUTABLE")
            get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)
            if (DLL STREQUAL "ON")
                string(FIND ${target} "minko-plugin" TEST_PLUGIN)
                if (TEST_PLUGIN STREQUAL -1)
                    foreach (DLL ${WINDOWS_DLL})
                        file(COPY "${DLL}" DESTINATION "${OUTPUT_PATH}")
                    endforeach ()
                endif ()
            else ()
                foreach (DLL ${WINDOWS_DLL})
                    file(COPY "${DLL}" DESTINATION "${OUTPUT_PATH}")
                endforeach ()
            endif ()
        endif ()
    elseif (LINUX)
        target_link_libraries(${target}
            "SDL2"
        )
    elseif (APPLE AND NOT IOS)
        target_link_libraries(${target}
            "${SDL_PATH}/lib/sdl/lib/osx64/libSDL2.a"
            "-framework CoreAudio"
            "-framework Carbon"
            "-framework AudioUnit"
            "-framework ForceFeedback"
        )
        link_directories("${SDL_PATH}/lib/sdl/lib/osx64")
        link_directories("${SDL_PATH}/lib/sdl/lib/ios")
    elseif (IOS)
        target_link_libraries(${target}
            "${SDL_PATH}/lib/sdl/lib/ios/libSDL2.a"
      	    "${SDL_PATH}/lib/sdl/lib/ios/libSDL2_mixer.a"
            "-framework CoreAudio"
            "-framework AudioToolbox"
        )
    elseif (ANDROID)
        link_directories("${SDL_PATH}/lib/sdl/lib/android")
        target_link_libraries(${target}
            "${SDL_PATH}/lib/sdl/lib/android/libSDL2_mixer.a"
            "${SDL_PATH}/lib/sdl/lib/android/libSDL2.a"
        )
        target_include_directories(${target} PRIVATE
            "${SDL_PATH}/lib/sdl/src/core/android"
        )
    endif ()
    if (EMSCRIPTEN)
        target_link_libraries(${target}
            "SDL"
        )
    endif ()
    # add requested offscreen
    if (WITH_OFFSCREEN STREQUAL "ON" OR WITH_OFFSCREEN STREQUAL "ON")
        minko_enable_plugin_offscreen (${target})
    endif ()
endfunction ()