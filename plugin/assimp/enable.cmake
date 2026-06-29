function (minko_enable_plugin_assimp target)
    set (ASSIMP_PATH "${MINKO_HOME}/plugin/assimp")
    # Enabling zlib by default my cause multiple definition linking errors.
    # So the app CMakeLists.txt must enable the zlib plugin on a case-by-case basis.
    # minko_enable_plugin_zlib (${target})
    minko_plugin_link ("assimp" ${target})
    string (FIND ${target} "minko-example" TEST_EXAMPLE)
    string (FIND ${target} "minko-plugin" TEST_PLUGIN)
    if (TEST_EXAMPLE EQUAL -1 AND TEST_PLUGIN EQUAL -1)
        # Link the vendored Assimp static library by its known output path rather
        # than find_library (which resolves at configure time, before the plugin
        # build has produced libassimp.a). The path form is a plain link-time
        # reference. For test/example/plugin targets minko_plugin_link adds a
        # target-level dependency that orders the build; plain app targets rely
        # on Minko's staged SDK-then-app build, like the pre-existing
        # Emscripten/Android paths.
        target_link_libraries (${target} "${MINKO_HOME}/plugin/assimp/bin/libassimp.a")
    endif ()
    target_include_directories(${target} PRIVATE "${ASSIMP_PATH}/include")
    target_compile_options(${target} PRIVATE -DMINKO_PLUGIN_ASSIMP)
endfunction ()