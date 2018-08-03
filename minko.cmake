function (configure_target_flags target)

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (BITNESS 64 PARENT_SCOPE)
        set (BITNESS 64)
    else ()
        set (BITNESS 32 PARENT_SCOPE)
        set (BITNESS 32)
    endif ()

    if (NOT CMAKE_BUILD_TYPE)
        set (CMAKE_BUILD_TYPE "Debug" PARENT_SCOPE)
        set (CMAKE_BUILD_TYPE "Debug")
    endif ()

    string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)

    if (UNIX AND NOT EMSCRIPTEN)
        set (COMPILATION_FLAGS -MMD -m${BITNESS})
    endif ()
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} -MP -DJSON_IS_AMALGAMATION)
    if (EMSCRIPTEN)
        set (SYSTEM_NAME "asmjs")
        if (WASM)
            set (SYSTEM_NAME "wasm")
        endif ()
        set (COMPILATION_FLAGS ${COMPILATION_FLAGS} -MMD -DEMSCRIPTEN)
    endif()

    set (OUTPUT_PATH
        "${CMAKE_CURRENT_BINARY_DIR}/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}"
        PARENT_SCOPE
    )
    set (OUTPUT_PATH
        "${CMAKE_CURRENT_BINARY_DIR}/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}"
    )
    target_compile_options(${target} 
        PUBLIC 
        ${COMPILATION_FLAGS}
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} PARENT_SCOPE)
    set_target_properties(${target}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG
        ${OUTPUT_PATH}
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE
        ${OUTPUT_PATH}
        ARCHIVE_OUTPUT_DIRECTORY
        ${OUTPUT_PATH}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG
        ${OUTPUT_PATH}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE
        ${OUTPUT_PATH}
        RUNTIME_OUTPUT_DIRECTORY
        ${OUTPUT_PATH}
        LIBRARY_OUTPUT_DIRECTORY_DEBUG
        ${OUTPUT_PATH}
        LIBRARY_OUTPUT_DIRECTORY_RELEASE
        ${OUTPUT_PATH}
        LIBRARY_OUTPUT_DIRECTORY
        ${OUTPUT_PATH}
    )
endfunction ()