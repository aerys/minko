function (enable_sdl target)
    target_link_libraries(${target} minko-plugin-${name})

    #plugin_link("sdl" target)
    add_definitions("-DMINKO_PLUGIN_SDL")
    set(SDL_PATH "${MINKO_HOME}/plugin/sdl")
    list (APPEND
        SDL_INCLUDE 
        "${SDL_PATH}/include"
        "${SDL_PATH}/src"
    )
    if (NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        list (APPEND 
            SDL_INCLUDE
            "${SDL_PATH}/lib/sdl/include"
        )
    endif ()
    target_include_directories(${target}
        PUBLIC
        ${SDL_INCLUDE}
    )
    if (WIN32)
        target_link_libraries(${target}
            "SDL2"
            "SDL2main"
            "SDL2_mixer"
        )
        if (BITNESS EQUAL 32)
            link_directories("{SDL_PATH}/lib/sdl/lib/windows32")
            configure_file("{SDL_PATH}/lib/sdl/lib/windows32/*.dll" "${CMAKE_CURRENT_SOURCE_DIR}" COPYONLY)
        else ()
            link_directories("{SDL_PATH}/lib/sdl/lib/windows64")
            configure_file("{SDL_PATH}/lib/sdl/lib/windows64/*.dll" "${CMAKE_CURRENT_SOURCE_DIR}" COPYONLY)
        endif ()
    elseif (UNIX AND NOT APPLE)
        target_link_libraries(${target}
            "SDL2"
        )
    elseif (APPLE)
        target_link_libraries(${target}
            "SDL2"
            "SDL2_mixer"
            "CoreAudio.framework"
            "AudioToolbox.framework"
            # "CoreFoundation.framework"
            "Carbon.framework"
            "AudioUnit.framework"
            "ForceFeedback.framework"
        )
        link_directories("{SDL_PATH}/lib/sdl/lib/osx64")
        link_directories("{SDL_PATH}/lib/sdl/lib/ios")
    elseif (ANDROID)
        target_link_libraries(${target}
            "SDL2"
            "SDL2_mixer"
        )
        link_directories("{SDL_PATH}/lib/sdl/lib/android")
        target_include_directories(${target}
            "${SDL_PATH}/lib/sdl/src/core/android"
        )
    endif ()
    # add off screen
endfunction ()