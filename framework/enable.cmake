function (minko_enable_framework target)
    minko_package_assets(
        ${target}
        EMBED
        "${MINKO_HOME}/framework/asset/effect/*.glsl"
        "${MINKO_HOME}/framework/asset/effect/*.effect"
    )

    if (WIN32)
        target_link_libraries (${target} "${MINKO_HOME}/framework/bin/minko-framework.lib")
    else ()
        target_link_libraries(${target} "${MINKO_HOME}/framework/bin/libminko-framework.a")
    endif ()

    list (APPEND FRAMEWORK_INCLUDES
        "${MINKO_HOME}/framework/include"
        "${MINKO_HOME}/framework/lib/glm"
        "${MINKO_HOME}/framework/lib/sparsehash/src"
        "${MINKO_HOME}/framework/lib/json/include"
    )
    if (WIN32)
        list (
            APPEND
            FRAMEWORK_INCLUDES
            "${MINKO_HOME}/framework/lib/sparsehash/include/windows"
        )
    else ()
        list (
            APPEND
            FRAMEWORK_INCLUDES
            "${MINKO_HOME}/framework/lib/sparsehash/include"
        )
    endif ()

    target_include_directories (${target} PRIVATE "${FRAMEWORK_INCLUDES}")

    if (WIN32)
        target_include_directories (${target_name}
            PUBLIC 
            "${MINKO_HOME}/framework/lib/sparsehash/include/windows"
            "${MINKO_HOME}/framework/lib/glew/include"
        )
        target_compile_options (${target_name} PUBLIC "/wd4996")

        find_library (GLEW32_LIB glew32 HINTS "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}")
        target_link_libraries (
            ${target_name}
            "OpenGL32"
            ${GLEW32_LIB}
        )
        file (
            GLOB
            WINDOWS_DLL
            "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}/*.dll"
        )
        target_include_directories (
            ${target_name}
            PUBLIC
            "${MINKO_HOME}/framework/lib/glew/include"
        )

        get_target_property(TARGET_TYPE ${target} TYPE)
        if (TARGET_TYPE STREQUAL "EXECUTABLE")
            get_target_property(OUTPUT_PATH ${target} RUNTIME_OUTPUT_DIRECTORY)

            foreach (DLL ${WINDOWS_DLL})
                file (COPY "${DLL}" DESTINATION "${OUTPUT_PATH}")
            endforeach ()
        endif ()
    else ()
        target_include_directories (${target_name} PUBLIC "/framework/lib/sparsehash/include")
    endif ()
endfunction()
