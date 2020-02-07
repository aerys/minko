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
            target_compile_options(${target_name} PRIVATE -O3 --llvm-lto 1)
        else ()
            target_compile_options(${target_name} PRIVATE --llvm-opts 0 -g4)
            set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -g4")
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

    minko_enable_framework(${target_name})
    minko_configure_target_flags (${target_name})

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

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (BITNESS 64)
    else ()
        set (BITNESS 32)
    endif ()

    list (
        APPEND
        FRAMEWORK_INCLUDES
        "${MINKO_HOME}/framework/include"
        "${MINKO_HOME}/framework/lib/glm"
        "${MINKO_HOME}/framework/lib/sparsehash/src"
        "${MINKO_HOME}/framework/lib/jsonmoderncpp"
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
        target_compile_options (${target_name} PUBLIC "-std=c++11")
    endif ()

    if (EMSCRIPTEN)
        set (WASM_OPTS  -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s "BINARYEN_TRAP_MODE='clamp'")
        set (ASMJS_OPTS -s WASM=0 -s ALLOW_MEMORY_GROWTH=0)

        if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/template.html)
            set (SHELL_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/template.html)
        else ()
            set (SHELL_FILE_PATH ${MINKO_HOME}/template/template.html)
        endif ()

        # Emscripten build is done in two steps:
        # 1. build the bytecode (*.bc)
        # 2. convert the bytecode to JavaScript (*.js)
        set_target_properties (${target_name} PROPERTIES SUFFIX ".bc")

        if (CMAKE_BUILD_TYPE STREQUAL "debug" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
            set_target_properties(${target_name} PROPERTIES LINK_FLAGS "--llvm-lto 0 -g4 -Wl --no-as-needed")
            target_compile_options(${target_name} PRIVATE "-g4")
            add_custom_command (
                TARGET ${target_name}
                POST_BUILD
                # Compile the Emscripten (*.bc) bytecode to JavaScript (*.js)
                COMMAND $ENV{EMSCRIPTEN}/em++
                    ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.bc
                    -o ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}-asmjs.js
                    -g4
                    --js-library ${MINKO_HOME}/cmake/library.js
                    --memory-init-file 1
                    -s DISABLE_EXCEPTION_CATCHING=0
                    -s DEMANGLE_SUPPORT=1
                    -s NO_EXIT_RUNTIME=1
                    -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\"
                    -s EXPORTED_RUNTIME_METHODS="[\'FS_createPath\',\'FS_createDataFile\',\'addRunDependency\',\'removeRunDependency\',\'getMemory\',\'cwrap\']"
                    -s FORCE_FILESYSTEM=1
                    -s ERROR_ON_UNDEFINED_SYMBOLS=0
                    ${ASMJS_OPTS}
                COMMAND $ENV{EMSCRIPTEN}/em++
                    ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.bc
                    -o ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}-wasm.js
                    -g4
                    --js-library ${MINKO_HOME}/cmake/library.js
                    --memory-init-file 1
                    -s DISABLE_EXCEPTION_CATCHING=0
                    -s DEMANGLE_SUPPORT=1
                    -s NO_EXIT_RUNTIME=1
                    -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\"
                    -s EXPORTED_RUNTIME_METHODS="[\'FS_createPath\',\'FS_createDataFile\',\'addRunDependency\',\'removeRunDependency\',\'getMemory\',\'cwrap\']"
                    -s FORCE_FILESYSTEM=1
                    -s ERROR_ON_UNDEFINED_SYMBOLS=0
                    ${WASM_OPTS}
                # Copy the HTML template.
                COMMAND ${CMAKE_COMMAND}
                    -E copy \"${SHELL_FILE_PATH}\" ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.html
                # Generate the *.data + *-preload.js for the embedded assets
                COMMAND python
                    ${MINKO_HOME}/cmake/empkg.py
                    ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.data
            )
        else ()
            set_target_properties(${target_name} PROPERTIES LINK_FLAGS "--llvm-lto 1 -Wl --no-as-needed")
            add_custom_command (
                TARGET ${target_name}
                POST_BUILD
                # Compile the Emscripten (*.bc) bytecode to JavaScript (*.js)
                COMMAND $ENV{EMSCRIPTEN}/em++
                    ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.bc
                    -o ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}-asmjs.js
                    -O3
                    --js-library ${MINKO_HOME}/cmake/library.js
                    --memory-init-file 1
                    -s DISABLE_EXCEPTION_CATCHING=1
                    -s NO_EXIT_RUNTIME=1
                    -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\"
                    -s EXPORTED_RUNTIME_METHODS="[\'FS_createPath\',\'FS_createDataFile\',\'addRunDependency\',\'removeRunDependency\',\'getMemory\',\'cwrap\']"
                    -s FORCE_FILESYSTEM=1
                    -s ERROR_ON_UNDEFINED_SYMBOLS=0
                    ${ASMJS_OPTS}
                COMMAND $ENV{EMSCRIPTEN}/em++
                    ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.bc
                    -o ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}-wasm.js
                    -O3
                    --js-library ${MINKO_HOME}/cmake/library.js
                    --memory-init-file 1
                    -s DISABLE_EXCEPTION_CATCHING=1
                    -s NO_EXIT_RUNTIME=1
                    -s EXPORTED_FUNCTIONS=\"[\'_main\', \'_minkoRunPlayer\']\"
                    -s EXPORTED_RUNTIME_METHODS="[\'FS_createPath\',\'FS_createDataFile\',\'addRunDependency\',\'removeRunDependency\',\'getMemory\',\'cwrap\']"
                    -s FORCE_FILESYSTEM=1
                    -s ERROR_ON_UNDEFINED_SYMBOLS=0
                    ${WASM_OPTS}
                # Copy the HTML template.
                COMMAND ${CMAKE_COMMAND}
                    -E copy \"${SHELL_FILE_PATH}\" ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.html
                # Generate the *.data + *-preload.js for the embedded assets
                COMMAND python
                    ${MINKO_HOME}/cmake/empkg.py
                    ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}.data
            )
        endif ()
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
                -s TOTAL_MEMORY=268435456
                -s EXPORTED_FUNCTIONS="[\'minkoWorkerEntryPoint\']"
        )
    endif ()
endfunction ()