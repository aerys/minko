if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
    set (LINUX 1)
endif ()

function (minko_set_bitness)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (BITNESS 64 PARENT_SCOPE)
    else ()
        set (BITNESS 32 PARENT_SCOPE)
    endif ()
endfunction ()

function (minko_set_variables)

    minko_set_bitness()
    set (BITNESS ${BITNESS} PARENT_SCOPE)

    if (NOT CMAKE_BUILD_TYPE)
        set (CMAKE_BUILD_TYPE "Debug" PARENT_SCOPE)
        set (CMAKE_BUILD_TYPE "Debug")
    endif ()

    string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)
    set (BUILD_TYPE ${BUILD_TYPE} PARENT_SCOPE)

    if (EMSCRIPTEN)
        set (SYSTEM_NAME "asmjs")
        if (WASM)
            set (SYSTEM_NAME "wasm")
        endif ()
    endif ()

    set (SYSTEM_NAME ${SYSTEM_NAME} PARENT_SCOPE)
endfunction ()

function (minko_configure_target_flags target)
    if (UNIX AND NOT EMSCRIPTEN AND NOT APPLE)
        set (COMPILATION_FLAGS -MMD -m${BITNESS})
    elseif (APPLE)
        set (COMPILATION_FLAGS -MMD)
    endif ()
    
    if (EMSCRIPTEN)
        set (COMPILATION_FLAGS ${COMPILATION_FLAGS} -DEMSCRIPTEN)
    endif()

    target_compile_options (${target} PUBLIC ${COMPILATION_FLAGS} $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>)
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} PARENT_SCOPE)
    set_target_properties (
        ${target}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/bin"
    )
endfunction ()
