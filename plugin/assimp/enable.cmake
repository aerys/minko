function (minko_enable_plugin_assimp target)
    set (ASSIMP_PATH "${MINKO_HOME}/plugin/assimp")
    minko_enable_plugin_zlib (${target})
    minko_plugin_link ("assimp" ${target})
    string (FIND ${target} "minko-example" TEST_EXAMPLE)
    string (FIND ${target} "minko-plugin" TEST_PLUGIN)
    if (TEST_EXAMPLE EQUAL -1 AND TEST_PLUGIN EQUAL -1)
        if (NOT EMSCRIPTEN AND NOT ANDROID)
            find_library (
                LIBASSIMP_LIB
                NAMES libassimp 
                HINTS "${MINKO_HOME}/plugin/assimp/bin/"
            )
            target_link_libraries (${target} ${LIBASSIMP_LIB})
        else ()
            target_link_libraries (${target} "${MINKO_HOME}/plugin/assimp/bin/libassimp.a")
        endif ()
    endif ()
    target_include_directories(${target} PRIVATE "${ASSIMP_PATH}/include")
    target_compile_options(${target} PRIVATE -DMINKO_PLUGIN_ASSIMP)
endfunction ()