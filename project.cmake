function (project_library target)
    list (APPEND FRAMEWORK_INCLUDES
        "${MINKO_HOME}/framework/include"
        "${MINKO_HOME}/framework/lib/glm"
        "${MINKO_HOME}/framework/lib/sparsehash/src"
        "${MINKO_HOME}/framework/lib/jsoncpp/src"
    )
    target_include_directories(${target} PUBLIC ${FRAMEWORK_INCLUDES})
    if (WIN32)
        target_include_directories(${target}
            PUBLIC 
            "${MINKO_HOME}/framework/lib/sparsehash/include/windows"
            "${MINKO_HOME}/framework/lib/glew/include"
        )
        target_compile_options(${target} PUBLIC "/wd4996")
    else ()
        target_include_directories(${target} PUBLIC "/framework/lib/sparsehash/include")
    endif ()

    if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PUBLIC "-DDEBUG")
    else ()
        target_compile_options(${target} PUBLIC "-DNDEBUG")
    endif ()

    if (MSVC)
        target_compile_options(${target}
            PUBLIC 
            "-DNOMINMAX" 
            "-D_VARIADIC_MAX=10" 
            "-D_USE_MATH_DEFINES" 
            "-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS"
            "/wd4503"
            )
    endif ()

    if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            set_target_properties(${target}
                PROPERTIES
                COMPILE_FLAGS 
                "-O3 --llvm-lto 1"
            )
        else ()
            set_target_properties(${target}
                PROPERTIES
                COMPILE_FLAGS 
                "-O2 --llvm-opts 0 --js-opts 0 -g4" 
            )
        endif ()
    endif ()
endfunction ()

function(project_application target)
    if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PUBLIC "-DDEBUG")
    else ()
        target_compile_options(${target} PUBLIC "-DNDEBUG")
    endif ()
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (BITNESS 64)
    else ()
        set (BITNESS 32)
    endif ()
    package_assets(".glsl;.effect" off)
    string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
    string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)
    link_directories("${MINKO_HOME}/framework/bin/${SYSTEM_NAME}${BITNESS}/${BUILD_TYPE}")
    if (WIN32)
        find_library(GLEW32_LIB glew32 HINTS "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}")
        target_link_libraries(${target}
            "minko-framework"
            "OpenGL32"
            ${GLEW32_LIB}
        )
        file (GLOB
            WINDOWS_DLL
            "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}/*.dll"
        )
        foreach (DLL ${WINDOWS_DLL})
            configure_file("${DLL}" "${OUTPUT_PATH}" COPYONLY)
        endforeach ()
    endif ()
    if (UNIX AND NOT APPLE AND NOT ANDROID)
        set (CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl --no-as-needed")
        link_directories ("/usr/lib64")
        target_link_libraries(${target}
            "minko-framework"
            "GL"
            "m"
            "pthread"
        )
    elseif (APPLE)
        target_link_libraries(${target}
            "minko-framework"
            "m"
            "Cocoa.framework"
            "OpenGL.framework"
            "IOKit.framework"
        )
    endif ()
    if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        target_link_libraries(${target}
            "minko-framework"
        )
        if (WASM)
            set (NB_WASM 1)
        else ()
            set (NB_WASM 0)
        endif ()
        set_target_properties (${target} PROPERTIES LINK_FLAGS "-Wl --no-as-needed -s USE_SDL=2  -s FORCE_FILESYSTEM=1 -o ${OUTPUT_PATH}/${PROJECT_NAME}.bc -s WASM=${NB_WASM}")
        set_target_properties (${target} PROPERTIES SUFFIX ".bc")
        if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_custom_command(TARGET
                ${target}
                POST_BUILD
                COMMAND $ENV{EMSCRIPTEN}/emcc ${OUTPUT_PATH}/${PROJECT_NAME}.bc -o ${OUTPUT_PATH}/${PROJECT_NAME}.html --js-library ${MINKO_HOME}/module/emscripten/library.js --memory-init-file 1 -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\" -O3 --closure 1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s DISABLE_EXCEPTION_CATCHING=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s FORCE_FILESYSTEM=1 -s USE_SDL=2 --preload-file ${OUTPUT_PATH}/embed/asset@asset --shell-file \"${MINKO_HOME}/skeleton/template.html\" -s WASM=${NB_WASM}
            )

        else ()
            add_custom_command(TARGET
            ${target}
            POST_BUILD
            COMMAND $ENV{EMSCRIPTEN}/emcc ${OUTPUT_PATH}/${PROJECT_NAME}.bc -o ${OUTPUT_PATH}/${PROJECT_NAME}.html --js-library ${MINKO_HOME}/module/emscripten/library.js --memory-init-file 1 -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\" -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 -s DEMANGLE_SUPPORT=1  -s FORCE_FILESYSTEM=1 -s USE_SDL=2 --preload-file ${OUTPUT_PATH}/embed/asset@asset --shell-file \"${MINKO_HOME}/skeleton/template.html\" -s WASM=${NB_WASM}
        )
        endif ()
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/template.html")
            target_compile_options(${target}
                PUBLIC
                "--shell-file \"${CMAKE_CURRENT_SOURCE_DIR}/template.html\""
            )
        else ()
            target_compile_options(${target}
                PUBLIC
                "--shell-file \"${MINKO_HOME}/skeleton/template.html\""
            )
        endif ()
    endif ()
    if (APPLE)
        target_link_libraries(${target}
            "minko-framework"
            "m"
            "OpenGLES.framework"
            "Foundation.framework"
            "UIKit.framework"
            "QuartzCore.framework"
            "CoreGraphics.framework"
        )
        file (GLOB_RECURSE 
            APPLE_SRC
            "*.plist"
        )
        target_sources(${target} PUBLIC ${APPLE_SRC})
    endif ()
    if (ANDROID)
        add_custom_command(TARGET ${target}
            PRE_LINK
            COMMAND ${MINKO_HOME}/script/cpjf.sh ${CMAKE_CURRENT_SOURCE_DIR}/src ${OUTPUT_PATH}/src/com/minko
        )
        target_link_libraries(${target}
            "minko-framework"
            "GLESv1_CM"
            "GLESv2"
            "EGL"
            "dl"
            "z"
            "log"
            "android"
            "stdc++"
        )
        #set_target_properties (${target} PROPERTIES PREFIX "lib")
        #set_target_properties (${target} PROPERTIES SUFFIX ".so")
        set_target_properties (${target} PROPERTIES LINK_FLAGS 
            "-Wl -shared -pthread -Wl,--no-undefined -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit"
        )
        #set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif ()
endfunction()
