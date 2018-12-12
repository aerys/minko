function (minko_plugin_link plugin_name target)
    string (FIND ${target} "minko-example" CHECK_EXAMPLE)
    string (FIND ${target} "minko-plugin" CHECK_PLUGIN)
    string (FIND ${target} "minko-framework" CHECK_FRAMEWORK)
    string (FIND ${target} "minko-test" CHECK_TEST)
    if (CHECK_EXAMPLE EQUAL -1 AND CHECK_PLUGIN EQUAL -1 AND CHECK_FRAMEWORK EQUAL -1 AND CHECK_TEST EQUAL -1)
        if (NOT EMSCRIPTEN AND NOT ANDROID)
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

    if (ANDROID)
        cpjf ("${MINKO_HOME}/plugin/${plugin_name}/src" "${OUTPUT_PATH}" ${target})
    endif ()
endfunction ()