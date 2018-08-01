function (build_android target target_name)

    set (APP_NAME "Minko Example Cube")
    set (PACKAGE "com.minko.example.cube")
    set (ARTIFACT_NAME "minko-example-cube")

    # define all the needed variables
    set (ANDROID_HOME "/opt/android-sdk-linux")
    set (ADB "${ANDROID_HOME}/platform-tools/adb")
    set (VERSION "0")
    set (TEMPLATE_PATH "${MINKO_HOME}/template/android")
    set (ANDROID_KEYSTORE_ALIAS "myalias")
    set (ANDROID_KEYSTORE_PASSWORD "passwd")
    set (ANDROID_KEYSTORE_PATH "/root/my-release-key.keystore")
    # needs to get changed
    set (ARTIFACT_PATH "${OUTPUT_PATH}/bin/${ARTIFACT_NAME}-release.apk")
    
    # regex commands

    # execute_process(COMMAND
    #    sed -r "s/lib(.*).so/\\1/;s/-/ /g;s/([A-Za-z])([A-Za-z]+)/\\U\\1\\L\\2/g;s/([0-9]+)//g;s/[^[:alpha:]]\\s//g" "<<<" ${target_name}
    #    OUTPUT_VARIABLE
    #    APP_NAME
    # )

    # string(REGEX MATCHALL "s/lib(.*).so/\\1/;s/-/ /g;s/([A-Za-z])([A-Za-z]+)/\\U\\1\\L\\2/g;s/([0-9]+)//g;s/[^[:alpha:]]\\s//g" APP_NAME "libminko-example-cube.so")

    # message("LOL: ${APP_NAME}")

    # execute_process(COMMAND
    #    sed -r "s/lib(.*).so/com.\\1/;s/-/\\./g;s/\\.([0-9]+)//g;s/(.*)/\\L\\1/" "<<<" ${target_name}
    #    OUTPUT_VARIABLE
    #    PACKAGE
    # )
    # execute_process(COMMAND
    #    sed -r "s/ /-/g;s/(.*)/\\L\\1/" "<<<" ${APP_NAME}
    #    OUTPUT_VARIABLE
    #    ARTIFACT_NAME
    # )

    # pushd "${TARGET_DIR}" > /dev/nul

    add_custom_command(TARGET ${target}
        POST_BUILD
        COMMAND rsync -vr "${MINKO_HOME}/build/example/cube/bin/android32" .
    )

    string(REGEX REPLACE "[.]" "/" FORMATED_PACKAGE ${PACKAGE})
    add_custom_command(TARGET ${target}
        POST_BUILD
        COMMAND cp -r ${MINKO_HOME}/template/android/* ${OUTPUT_PATH}
        COMMAND mkdir -p "${OUTPUT_PATH}/src/${FORMATED_PACKAGE}"
        COMMAND mv "${OUTPUT_PATH}/src/*.java" "${OUTPUT_PATH}/src/${FORMATED_PACKAGE}"
        COMMAND sed -i 's/{{APP_NAME}}/${APP_NAME}/' ${OUTPUT_PATH}/res/values/strings.xml ${OUTPUT_PATH}/build.xml
        COMMAND sed -i 's/{{PACKAGE}}/${PACKAGE}/' ${OUTPUT_PATH}/AndroidManifest.xml ${OUTPUT_PATH}/src/${FORMATED_PACKAGE}/*.java
        COMMAND sed -i 's/{{VERSION_CODE}}/${VERSION}/' ${OUTPUT_PATH}/AndroidManifest.xml
        
        # COMMAND mkdir -p ${OUTPUT_PATH}/libs/armeabi/ && cp ${OUTPUT_PATH}/*.so ${OUTPUT_PATH}/libs/armeabi/ && mv ${OUTPUT_PATH}/libs/armeabi/${target_name} ${OUTPUT_PATH}/libs/armeabi/libmain.so
        COMMAND mkdir -p ${OUTPUT_PATH}/libs/armeabi-v7a/ && cp ${OUTPUT_PATH}/*.so ${OUTPUT_PATH}/libs/armeabi-v7a/ && mv ${OUTPUT_PATH}/libs/armeabi-v7a/${target_name} ${OUTPUT_PATH}/libs/armeabi-v7a/libmain.so
        # COMMAND mkdir -p ${OUTPUT_PATH}/libs/x86/ && cp ${OUTPUT_PATH}/*.so ${OUTPUT_PATH}/libs/x86/ && mv ${OUTPUT_PATH}/libs/x86/${target_name} ${OUTPUT_PATH}/libs/x86/libmain.so
        # COMMAND mkdir -p ${OUTPUT_PATH}/libs/x86_64/ && cp ${OUTPUT_PATH}/*.so ${OUTPUT_PATH}/libs/x86_64/ && mv ${OUTPUT_PATH}/libs/x86_64/${target_name} ${OUTPUT_PATH}/libs/x86_64/libmain.so
        # COMMAND mkdir -p ${OUTPUT_PATH}/libs/arm64-v8a/ && cp ${OUTPUT_PATH}/*.so ${OUTPUT_PATH}/libs/arm64-v8a/ && mv ${OUTPUT_PATH}/libs/arm64-v8a/${target_name} ${OUTPUT_PATH}/libs/arm64-v8a/libmain.so

        COMMAND rm -rf ${OUTPUT_PATH}/assets
        COMMAND mv ${OUTPUT_PATH}/asset ${OUTPUT_PATH}/assets
        COMMAND chmod u+rwx -R ${OUTPUT_PATH}/assets
        # make cmake buildtype to lower
        COMMAND ant "release"
        WORKING_DIRECTORY ${OUTPUT_PATH}
    )
    if (${CMAKE_BUILD_TYPE} STREQUAL "debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set (UNSIGNED_APK_PATH "${OUTPUT_PATH}/bin/${APP_NAME}-${CMAKE_BUILD_TYPE}.apk")
    else ()
        # need to get replaced
        set (UNSIGNED_APK_PATH "${OUTPUT_PATH}/bin/${APP_NAME}-release-unsigned.apk")
    endif ()
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "release")
        add_custom_command(TARGET ${target}
            POST_BUILD
            COMMAND jarsigner -tsa "http://timestamp.digicert.com" -keystore ${ANDROID_KEYSTORE_PATH} -storepass ${ANDROID_KEYSTORE_PASSWORD} -verbose -sigalg SHA1withRSA -digestalg SHA1 ${UNSIGNED_APK_PATH} ${ANDROID_KEYSTORE_ALIAS}
            COMMAND jarsigner -verify -verbose -certs ${UNSIGNED_APK_PATH}
            COMMAND zipalign -fv 4 ${UNSIGNED_APK_PATH} ${ARTIFACT_PATH}
            COMMAND rm -f ${UNSIGNED_APK_PATH}
        )
    else ()
        add_custom_command(TARGET ${target}
            POST_BUILD
            COMMAND mv ${UNSIGNED_APK_PATH} ${ARTIFACT_PATH}
        )
    endif ()

endfunction()