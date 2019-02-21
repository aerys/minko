function (cpjf src_dir dest_dir)
    file (
        GLOB_RECURSE
        JAVA_FILES
        "${src_dir}/*.java"
    )

    foreach (FILE ${JAVA_FILES})
        string (FIND ${FILE} "src/" DIR_SIZE)
        string (LENGTH ${FILE} LEN)
        math (EXPR RESULT ${LEN}-${DIR_SIZE}-4)
        math (EXPR DIR_SIZE ${DIR_SIZE}+4)
        string (SUBSTRING ${FILE} ${DIR_SIZE} ${RESULT} FINAL_DIR)
        file (COPY "${FILE}" DESTINATION "${dest_dir}/src/com/${FINAL_DIR}")
    endforeach ()
endfunction()

function (minko_plugin_link plugin_name target)
    get_target_property(OUTPUT_PATH ${target} LIBRARY_OUTPUT_DIRECTORY)

    string (FIND ${target} "minko-example" CHECK_EXAMPLE)
    string (FIND ${target} "minko-plugin" CHECK_PLUGIN)
    string (FIND ${target} "minko-framework" CHECK_FRAMEWORK)
    string (FIND ${target} "minko-test" CHECK_TEST)
    if (CHECK_EXAMPLE EQUAL -1 AND CHECK_PLUGIN EQUAL -1 AND CHECK_FRAMEWORK EQUAL -1 AND CHECK_TEST EQUAL -1)
        if (WIN32)
            find_library (
                ${plugin_name}_LIB
                NAMES minko-plugin-${plugin_name}
                HINTS "${MINKO_HOME}/plugin/${plugin_name}/bin/"
            )
            target_link_libraries (${target} ${${plugin_name}_LIB})
        else ()
            # The plugins will use symbols from the framework. But the linker will look for symbols in the
            # order of the libs as they are listed on the command line. To avoid undefined symbol errors,
            # we simply link with both the plugin and the framework in a "group".
            target_link_libraries(
                ${target}
                -Wl,--start-group
                ${MINKO_HOME}/plugin/${plugin_name}/bin/libminko-plugin-${plugin_name}.a
                ${MINKO_HOME}/framework/bin/libminko-framework.a
                -Wl,--end-group
            )
        endif ()
    else ()
        target_link_libraries (${target} minko-plugin-${plugin_name})
    endif ()

    if (ANDROID)
        cpjf ("${MINKO_HOME}/plugin/${plugin_name}/src" "${OUTPUT_PATH}")
    endif ()
endfunction ()
