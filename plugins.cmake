function (minko_plugin_link plugin_name target)
    string (FIND ${target} "minko-example" TEST_EXAMPLE)
    string (FIND ${target} "minko-plugin" TEST_PLUGIN)
    string (FIND ${target} "minko-framework" TEST_FRAMEWORK)
    if (TEST_EXAMPLE EQUAL -1 AND TEST_PLUGIN EQUAL -1 AND TEST_FRAMEWORK EQUAL -1)
        if (NOT EMSCRIPTEN)
            find_library (
                ${plugin_name}_LIB
                NAMES minko-plugin-${plugin_name} 
                HINTS "${MINKO_HOME}/build/plugin/${plugin_name}/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}"
            )
            target_link_libraries (${target} ${${plugin_name}_LIB})
        else ()
            target_link_libraries (${target} "${MINKO_HOME}/build/plugin/${plugin_name}/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}/libminko-plugin-${plugin_name}.a")
        endif ()
    else ()
        target_link_libraries (${target} minko-plugin-${plugin_name})
    endif ()
endfunction ()