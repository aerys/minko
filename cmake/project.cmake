function (minko_add_library target_name type sources)
    minko_set_variables()

    add_library (${target_name} ${type} "${sources}")

    minko_configure_target_flags (${target_name})
    # Don't enable the framework for itself.
    if (NOT ${target_name} STREQUAL "minko-framework")
        minko_enable_framework(${target_name})
    endif ()

    set (OUTPUT_PATH ${OUTPUT_PATH} PARENT_SCOPE)
    set (BITNESS ${BITNESS} PARENT_SCOPE)
    set (BUILD_TYPE ${BUILD_TYPE} PARENT_SCOPE)
    set (SYSTEM_NAME ${SYSTEM_NAME} PARENT_SCOPE)
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} PARENT_SCOPE)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)

    if (APPLE)
        target_compile_options(${target_name} PUBLIC -stdlib=libc++)
    endif ()

    if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options (${target_name} PUBLIC "-DDEBUG")
    else ()
        target_compile_options (${target_name} PUBLIC "-DNDEBUG")
    endif ()

    if (UNIX AND NOT APPLE AND NOT ANDROID)
        target_link_libraries (${target_name} "-lGL")
    endif ()
    if (ANDROID)
        target_link_libraries (${target_name}
            "GLESv1_CM"
            "GLESv2"
            "EGL"
            "dl"
            "z"
            "log"
            "android"
            "stdc++"
        )
        set_target_properties (
            ${target_name}
            PROPERTIES LINK_FLAGS
            "-Wl -shared -pthread -Wl,--no-undefined -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit"
        )
        target_compile_options (
            ${target_name}
            PUBLIC
            "-Wno-narrowing"
            "-Wno-tautological-compare"
            "-DEVL_EGLEXT_PROTOTYPES"
        )

        # The Android toolchain uses -g by default, which produces large binaries with debug
        # symbols even when CMAKE_BUILD_TYPE is set to Release. To fix this, we set -g again
        # but to 0.
        target_compile_options(${target_name} PUBLIC "-g0")
    endif ()

    if (IOS)
        set_target_properties (
            ${target_name}
            PROPERTIES
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET
            "7.0"
        )
    endif ()

    if (MSVC)
        target_compile_options(${target_name}
            PUBLIC
            "-DNOMINMAX"
            "-D_VARIADIC_MAX=10"
            "-D_USE_MATH_DEFINES"
            "-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS"
            "/wd4503"
        )
    endif ()

    if (EMSCRIPTEN)
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target_name} PRIVATE -O3 -flto)
        else ()
            target_compile_options(${target_name} PRIVATE -gsource-map)
            set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -gsource-map")
        endif ()
    endif ()
endfunction ()

# minko_add_executable function start
function (minko_add_executable target_name sources)
    if (ANDROID)
        minko_add_library(${target_name} SHARED "${sources}")
        build_android(${target_name} "lib${target_name}.so")

        return()
    endif()

    minko_set_variables()

    add_executable (${target_name} "${sources}")

    minko_configure_target_flags (${target_name})
    minko_enable_framework(${target_name})

    if (WITH_OFFSCREEN STREQUAL "ON" OR WITH_OFFSCREEN STREQUAL "ON")
        minko_enable_plugin_offscreen (${target_name})
    endif ()

    set (OUTPUT_PATH ${OUTPUT_PATH} PARENT_SCOPE)
    set (BITNESS ${BITNESS} PARENT_SCOPE)
    set (COMPILATION_FLAGS ${COMPILATION_FLAGS} PARENT_SCOPE)
    set (SYSTEM_NAME ${SYSTEM_NAME} PARENT_SCOPE)
    set (BUILD_TYPE ${BUILD_TYPE} PARENT_SCOPE)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)

    if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options (${target_name} PUBLIC "-DDEBUG")
    else ()
        target_compile_options (${target_name} PUBLIC "-DNDEBUG")
    endif ()

    minko_set_bitness()

    list (
        APPEND
        FRAMEWORK_INCLUDES
        "${MINKO_HOME}/framework/include"
        "${MINKO_HOME}/framework/lib/glm"
        "${MINKO_HOME}/framework/lib/sparsehash/src"
        "${MINKO_HOME}/framework/lib/jsoncpp/src"
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

    if (MSVC)
        target_compile_options(${target_name}
            PUBLIC
            "-DNOMINMAX"
            "-D_VARIADIC_MAX=10"
            "-D_USE_MATH_DEFINES"
            "-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS"
            "/wd4503"
        )
    endif ()

    if (UNIX AND NOT APPLE AND NOT ANDROID)
        link_directories ("/usr/lib64")
        target_link_libraries (
            ${target_name}
            "GL"
            "m"
            "-pthread"
        )
    elseif (APPLE AND NOT IOS)
        target_link_libraries (
            ${target_name}
            "m"
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework IOKit"
        )
    elseif (IOS)
        target_link_libraries (
            ${target_name}
            "m"
            "-framework OpenGLES"
            "-framework Foundation"
            "-framework UIKit"
            "-framework QuartzCore"
            "-framework CoreGraphics"
        )
        set_target_properties (${target_name} PROPERTIES XCODE_PRODUCT_TYPE com.apple.product-type.application)
        configure_file (${MINKO_HOME}/template/Info.plist ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist COPYONLY)
        configure_file (${MINKO_HOME}/template/Default-568h@2x.png ${CMAKE_CURRENT_SOURCE_DIR}/Default-568h@2x.png COPYONLY)
        file (GLOB_RECURSE IOS_SRC "*.plist")
        target_sources(${target_name} PUBLIC ${IOS_SRC})
    endif ()

    if (APPLE)
        target_compile_options (${target_name} PUBLIC "-std=c++14")
    endif ()

    if (EMSCRIPTEN)
        if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/template.html)
            set (SHELL_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/template.html)
        else ()
            set (SHELL_FILE_PATH ${MINKO_HOME}/template/template.html)
        endif ()

        set (CUSTOM_LINK_FLAGS "")

        if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
            string(JOIN " " CUSTOM_LINK_FLAGS
                "-gsource-map"
                "-s DISABLE_EXCEPTION_CATCHING=0"
                "-s DEMANGLE_SUPPORT=1"
                "-s SOCKET_DEBUG=1"
                "-s RETAIN_COMPILER_SETTINGS"
            )
            target_compile_options(${target_name} PRIVATE "-gsource-map")
        else ()
            string(JOIN " " CUSTOM_LINK_FLAGS
                "-flto"
                "-O3"
                "-s DISABLE_EXCEPTION_CATCHING=1"
                "-s RETAIN_COMPILER_SETTINGS"
            )
        endif ()

        string(JOIN " " CUSTOM_LINK_FLAGS
            "${CUSTOM_LINK_FLAGS}"
            "-Wl --no-as-needed"
            "--js-library ${MINKO_HOME}/cmake/library.js"
            "-s NO_EXIT_RUNTIME=1"
            "-s EXPORTED_FUNCTIONS=\"[\'_main\',\'_minkoRunPlayer\',\'_malloc\']\""
            "-s EXPORTED_RUNTIME_METHODS=\"[\'callMain\',\'FS_createPath\',\'FS_createDataFile\',\'addRunDependency\',\'removeRunDependency\',\'cwrap\']\""
            "-s FORCE_FILESYSTEM"
            "-s ERROR_ON_UNDEFINED_SYMBOLS=0"
            "-s LLD_REPORT_UNDEFINED"
            "-s USE_PTHREADS=0"
            "-s WASM=1"
            "-s ALLOW_MEMORY_GROWTH=1"
        )

        set_target_properties(
            ${target_name}
            PROPERTIES SUFFIX "-wasm.js"
            LINK_FLAGS "${CUSTOM_LINK_FLAGS}"
        )

        add_custom_command (
            TARGET ${target_name}
            POST_BUILD
            # Copy the HTML template.
            COMMAND ${CMAKE_COMMAND}
                -E copy \"${SHELL_FILE_PATH}\" ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.html
            # Generate the *.data + *-preload.js for the embedded assets
            COMMAND python3
                ${MINKO_HOME}/cmake/empkg.py
                ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.data
        )
    endif ()
endfunction ()
# minko_add_executable function end

function (minko_add_worker target_name sources)
    minko_add_library (${target_name} "STATIC" "${sources}")

    get_property (
        WORKER_LIBS
        TARGET ${target_name}
        PROPERTY LINK_LIBRARIES
    )
    list (
        REMOVE_ITEM
        WORKER_LIBS
        "minko-framework"
    )

    if (EMSCRIPTEN)
        add_custom_command (
            TARGET ${target_name}
            POST_BUILD
            COMMAND $ENV{EMSCRIPTEN}/emcc
                ${OUTPUT_PATH}/${PROJECT_NAME}
                -o ${OUTPUT_PATH}/${PROJECT_NAME}.js
                -O3
                --closure 1
                -s DISABLE_EXCEPTION_CATCHING=0
                -s INITIAL_MEMORY=268435456
                -s EXPORTED_FUNCTIONS="[\'minkoWorkerEntryPoint\']"
        )
    endif ()
endfunction ()
