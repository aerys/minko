function (project_library target)
    list (APPEND FRAMEWORK_INCLUDES
        "${MINKO_HOME}/framework/include"
        "${MINKO_HOME}/framework/lib/glm"
        "${MINKO_HOME}/framework/lib/sparsehash/src"
        "${MINKO_HOME}/framework/lib/jsoncpp/src"
    )
    target_include_directories(${target} PUBLIC ${FRAMEWORK_INCLUDES})
    target_compile_options(${target} PUBLIC "-DJSON_IS_AMALGAMATION")
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

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
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
                "-O3 --llvm-lto 1 -std=c++11"
            )
        else ()
            set_target_properties(${target}
                PROPERTIES
                COMPILE_FLAGS 
                "-O2 --llvm-opts 0 --js-opts 0 -g4 -std=c++11" 
            )
        endif ()
    endif ()
endfunction ()

function(project_application target)
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
        target_link_libraries(${target}
            "minko-framework"
            "OpenGL32"
            "glew32"
        )
        file (GLOB
            WINDOWS_DLL
            "${MINKO_HOME}/framework/lib/glew/lib/windows${BITNESS}/*.dll"
        )
        foreach (DLL ${WINDOWS_DLL})
            configure_file("${DLL}" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)
        endforeach ()
    endif ()
    if (UNIX AND NOT APPLE)
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
        set_target_properties (${target} PROPERTIES LINK_FLAGS "-Wl --no-as-needed -s USE_SDL=2")
        set_target_properties (${target} PROPERTIES SUFFIX ".html")
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set_target_properties (${target}
                PROPERTIES 
                COMPILE_FLAGS 
                # optimization
                "-DJSON_IS_AMALGAMATION -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.html -std=c++11 --js-library ${MINKO_HOME}/module/emscripten/library.js --memory-init-file 1 -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\" -O3 --closure 1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s DISABLE_EXCEPTION_CATCHING=1 -s ALLOW_MEMORY_GROWTH=0 -s NO_EXIT_RUNTIME=1 -s OUTLINING_LIMIT=20000"
                # set the app (or the sdk) template.html
            )
        else ()
            set_target_properties (${target}
                PROPERTIES 
                COMPILE_FLAGS 
                # treat undefined symbol warnings as errors
                # cmd = cmd .. ' -s ERROR_ON_UNDEFINED_SYMBOLS=1'
                # disable exception catching
                "-DJSON_IS_AMALGAMATION -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.html -std=c++11 --js-library ${MINKO_HOME}/module/emscripten/library.js --memory-init-file 1 -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\" -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=0 -s NO_EXIT_RUNTIME=1 -s DEMANGLE_SUPPORT=1"
                #[[
                optimize (very) long functions by breaking them into smaller ones
                from emscripten's settings.js:
                "OUTLINING_LIMIT: break up functions into smaller ones, to avoid the downsides of very
                large functions (JS engines often compile them very slowly, compile them with lower optimizations,
                -s OUTLINING_LIMIT=20000or do not optimize them at all)"
                ]]
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
        #[[add_custom_command(TARGET ${target}
            PRE_LINK
            COMMAND ${MINKO_HOME}/script/cpjf.sh ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src/com/minko
        )]]
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
        set_target_properties (${target} PROPERTIES PREFIX "lib")
        set_target_properties (${target} PROPERTIES PREFIX ".so")
        set_target_properties (${target} PROPERTIES LINK_FLAGS 
            "-Wl -shared -pthread -Wl,--no-undefined -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit"
        )
        # copy ("${MINKO_HOME}/template/android/*" ${CMAKE_CURRENT_SOURCE_DIR})
        #[[add_custom_command(TARGET ${target}
            POST_BUILD
            COMMAND ${MINKO_HOME}/script/build_android.sh ${target}
        )]]
    endif ()
endfunction()
