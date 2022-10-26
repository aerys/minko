function (build_android target target_name)
    # CMake toolchain to create and sign apks from shared libraries.

    # keystore warnings
    if ($ENV{ANDROID_KEYSTORE_PATH})
        message ("Warning: Missing environment variable ANDROID_KEYSTORE_PATH, generated APK will be unsigned.")
    endif ()

    if ($ENV{ANDROID_KEYSTORE_ALIAS})
        message ("Warning: Missing environment variable ANDROID_KEYSTORE_ALIAS, generated APK will be unsigned.")
    endif ()

    if ($ENV{ANDROID_KEYSTORE_PASSWORD})
        message ("Warning: Missing environment variable ANDROID_KEYSTORE_PASSWORD, generated APK will be unsigned.")
    endif ()

    get_target_property(OUTPUT_PATH ${target} LIBRARY_OUTPUT_DIRECTORY)

    if (DEFINED ENV{ANDROID_VERSION_CODE})
        set (VERSION_CODE $ENV{ANDROID_VERSION_CODE})
    else ()
        message ("Warning: Missing environment variable ANDROID_VERSION_CODE, generated APK will have the versionCode 0.")
        set (VERSION_CODE "0")
    endif()

    # use regex to get the names of the app, package and artifact
    string (REGEX REPLACE "lib(.*).so" "\\1" APP_CUT ${target_name})
    set (ARTIFACT_NAME ${APP_CUT})
    string (REGEX REPLACE "-" " " APP_NAME ${APP_CUT})
    string (REGEX REPLACE "lib(.*).so" "com.\\1" PACKAGE_CUT ${target_name})
    string (REGEX REPLACE "-" "." PACKAGE ${PACKAGE_CUT})

    string (REGEX REPLACE "\\." "/" FORMATED_PACKAGE ${PACKAGE})
    string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD)
    set (ARTIFACT_PATH "${OUTPUT_PATH}/bin/${ARTIFACT_NAME}-${BUILD}.apk")

    add_custom_command (
        TARGET ${target}
        POST_BUILD
        COMMAND cp -r ${MINKO_HOME}/template/android/* ${OUTPUT_PATH}
        WORKING_DIRECTORY ${OUTPUT_PATH}
    )
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/android)
        add_custom_command (
            TARGET ${target}
            POST_BUILD
            COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/android/* ${OUTPUT_PATH}
            WORKING_DIRECTORY ${OUTPUT_PATH}
        )
    endif ()
    
    add_custom_command (
        TARGET ${target}
        POST_BUILD
        COMMAND mkdir -p "${OUTPUT_PATH}/src/${FORMATED_PACKAGE}"
        COMMAND mv "${OUTPUT_PATH}/src/*.java" "${OUTPUT_PATH}/src/${FORMATED_PACKAGE}"
        COMMAND sed -i 's/{{APP_NAME}}/${APP_NAME}/' ${OUTPUT_PATH}/res/values/strings.xml ${OUTPUT_PATH}/build.xml
        COMMAND sed -i 's/{{PACKAGE}}/${PACKAGE}/' ${OUTPUT_PATH}/AndroidManifest.xml ${OUTPUT_PATH}/src/${FORMATED_PACKAGE}/*.java
        COMMAND sed -i 's/{{VERSION_CODE}}/${VERSION_CODE}/' ${OUTPUT_PATH}/AndroidManifest.xml 
        COMMAND mkdir -p ${OUTPUT_PATH}/libs/armeabi-v7a/ && cp ${OUTPUT_PATH}/*.so ${OUTPUT_PATH}/libs/armeabi-v7a/ && mv ${OUTPUT_PATH}/libs/armeabi-v7a/${target_name} ${OUTPUT_PATH}/libs/armeabi-v7a/libmain.so
        COMMAND rm -rf ${OUTPUT_PATH}/assets
        WORKING_DIRECTORY ${OUTPUT_PATH}
    )

    if (EXISTS "${OUTPUT_PATH}/asset")
        add_custom_command (
            TARGET ${target}
            COMMAND mv ${OUTPUT_PATH}/asset ${OUTPUT_PATH}/assets
            COMMAND chmod u+rwx -R ${OUTPUT_PATH}/assets
            WORKING_DIRECTORY ${OUTPUT_PATH}
    )
    endif ()
    add_custom_command (
        TARGET ${target}
        COMMAND ant "${BUILD}"
        WORKING_DIRECTORY ${OUTPUT_PATH}
    )
    
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "release")
        set (UNSIGNED_APK_PATH "${OUTPUT_PATH}/bin/${APP_NAME}-${BUILD}-unsigned.apk")
        
        # sign the apk (only on release mode)
        if (DEFINED ENV{ANDROID_KEYSTORE_PATH} AND DEFINED ENV{ANDROID_KEYSTORE_ALIAS} AND DEFINED ENV{ANDROID_KEYSTORE_PASSWORD})
            add_custom_command (
                TARGET ${target}
                POST_BUILD
                COMMAND jarsigner -tsa "http://timestamp.digicert.com" -keystore $ENV{ANDROID_KEYSTORE_PATH} -storepass $ENV{ANDROID_KEYSTORE_PASSWORD} -verbose -sigalg SHA1withRSA -digestalg SHA1 ${UNSIGNED_APK_PATH} $ENV{ANDROID_KEYSTORE_ALIAS}
                COMMAND jarsigner -verify -verbose -certs ${UNSIGNED_APK_PATH}
                COMMAND zipalign -fv 4 ${UNSIGNED_APK_PATH} ${ARTIFACT_PATH}
                COMMAND rm -f ${UNSIGNED_APK_PATH}
            )
        else ()
            message(WARNING "The ANDROID_KEYSTORE_PATH, ANDROID_KEYSTORE_ALIAS or ANDROID_KEYSTORE_PASSWORD environment variable is not set: APK will not be signed.")
        endif ()
    else ()
        set (UNSIGNED_APK_PATH "${OUTPUT_PATH}/bin/${APP_NAME}-${BUILD}.apk")
        add_custom_command (
            TARGET ${target}
            POST_BUILD
            COMMAND mv ${UNSIGNED_APK_PATH} ${ARTIFACT_PATH}
        )
    endif ()

    if (NOT ${ANDROID_STL} MATCHES "_shared")
        return()
    endif ()

    function(configure_shared_stl lib_path so_base)
        configure_file(
            "${ANDROID_NDK}/sources/cxx-stl/${lib_path}/libs/${ANDROID_ABI}/lib${so_base}.so" 
            "${OUTPUT_PATH}/libs/${ANDROID_ABI}/lib${so_base}.so" 
            COPYONLY
        )
    endfunction()

    if ("${ANDROID_STL}" STREQUAL "libstdc++")
        # The default minimal system C++ runtime library.
    elseif ("${ANDROID_STL}" STREQUAL "gabi++_shared")
        # The GAbi++ runtime (shared).
        message(FATAL_ERROR "gabi++_shared was not configured by ndk-stl package")
    elseif ("${ANDROID_STL}" STREQUAL "stlport_shared")
        # The STLport runtime (shared).
        configure_shared_stl("stlport" "stlport_shared")
    elseif ("${ANDROID_STL}" STREQUAL "gnustl_shared")
        # The GNU STL (shared).
        configure_shared_stl("gnu-libstdc++/4.9" "gnustl_shared")
    elseif ("${ANDROID_STL}" STREQUAL "c++_shared")
        # The LLVM libc++ runtime (static).
        configure_shared_stl("llvm-libc++" "c++_shared")
    else ()
        message(FATAL_ERROR "STL configuration ANDROID_STL=${ANDROID_STL} is not supported")
    endif ()
endfunction()