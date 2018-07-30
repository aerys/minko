function (build_android target target_name target_dir)

    # define all the needed variables
    set (ANDROID_HOME "/opt/android-sdk-linux")
    set (ADB "${ANDROID_HOME}/platform-tools/adb")
    set (VERSION "0")
    set (TEMPLATE_PATH "${MINKO_HOME}/template/android")
    set (ANDROID_KEYSTORE_ALIAS )
    set (ANDROID_KEYSTORE_PASSWORD )
    set (ANDROID_KEYSTORE_PATH )
    
    # regex commands
    execute_process(COMMAND
        sed -r 's/lib(.*).so/\1/;s/-/ /g;s/([A-Za-z])([A-Za-z]+)/\U\1\L\2/g;s/([0-9]+)//g;s/[^[:alpha:]]\s//g' <<< ${TARGET_NAME}
        OUTPUT_VARIABLE
        APP_NAME
    )
    execute_process(COMMAND
        sed -r 's/lib(.*).so/com.\1/;s/-/\./g;s/\.([0-9]+)//g;s/(.*)/\L\1/' <<< ${TARGET_NAME}
        OUTPUT_VARIABLE
        PACKAGE
    )
    execute_process(COMMAND
        sed -r 's/ /-/g;s/(.*)/\L\1/' <<< ${APP_NAME}
        OUTPUT_VARIABLE
        ARTIFACT_NAME
    )
    execute_process(
        COMMAND cp ${MINKO_HOME}/template/android/* ${OUTPUT_PATH}
        COMMAND temp_android_minko=${PACKAGE}
        COMMAND mkdir -p src/\$\{temp_android_minko//.//\}
        COMMAND mv src/*.java src/\$\{temp_android_minko//.//\}
        COMMAND sed -i 's/{{APP_NAME}}/${APP_NAME}/' ${OUTPUT_PATH}/res/values/strings.xml ${OUTPUT_PATH}/build.xml
        COMMAND sed -i 's/{{PACKAGE}}/${PACKAGE}/' ${OUTPUT_PATH}/AndroidManifest.xml ${OUTPUT_PATH}/src/\$\{temp_android_minko//.//\}/*.java
        COMMAND sed -i 's/{{VERSION_CODE}}/${VERSION_CODE}/' ${OUTPUT_PATH}/AndroidManifest.xml
        COMMAND mkdir -p ${OUTPUT_PATH}/libs/armeabi-v7a/ && cp ${OUTPUT_PATH}/*.so ${OUTPUT_PATH}/libs/armeabi-v7a/ && mv ${OUTPUT_PATH}/libs/armeabi-v7a/${target_name} ${OUTPUT_PATH}/libs/armeabi-v7a/libmain.so
        COMMAND rm -rf ${OUTPUT_PATH}/assets
        COMMAND mv ${OUTPUT_PATH}/asset ${OUTPUT_PATH}/assets
        COMMAND chmod u+rwx -R ${OUTPUT_PATH}/assets
        COMMAND cd ${OUTPUT_PATH}
        COMMAND ant ${CMAKE_BUILD_TYPE}
    )
    if (${CMAKE_BUILD_TYPE} STREQUAL "debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set (UNSIGNED_APK_PATH "bin/${APP_NAME}-${CMAKE_BUILD_TYPE}.apk")
    else ()
        set (UNSIGNED_APK_PATH "bin/${APP_NAME}-${CMAKE_BUILD_TYPE}-unsigned.apk")
    endif ()
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "release")
        execute_process(
            COMMAND jarsigner -tsa "http://timestamp.digicert.com" -keystore ${ANDROID_KEYSTORE_PATH} -storepass ${ANDROID_KEYSTORE_PASSWORD} -verbose -sigalg SHA1withRSA -digestalg SHA1 ${UNSIGNED_APK_PATH} ${ANDROID_KEYSTORE_ALIAS}
            COMMAND jarsigner -verify -verbose -certs ${UNSIGNED_APK_PATH}
            COMMAND zipalign -v -f 4 ${UNSIGNED_APK_PATH} ${ARTIFACT_PATH}
            COMMAND rm -f ${UNSIGNED_APK_PATH}
        )
    else ()
        execute_process(
            COMMAND mv ${UNSIGNED_APK_PATH} ${ARTIFACT_PATH}
        )
    endif ()

endfunction()