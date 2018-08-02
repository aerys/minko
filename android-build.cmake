function (build_android target target_name)

    # set (APP_NAME "Minko Example Cube")
    # set (PACKAGE "com.minko.example.cube")
    # set (ARTIFACT_NAME "minko-example-cube")

    # define all the needed variables
    set (ANDROID_HOME "/opt/android-sdk-linux")
    set (VERSION "0")
    set (ANDROID_KEYSTORE_ALIAS "myalias")
    set (ANDROID_KEYSTORE_PASSWORD "password")
    set (ANDROID_KEYSTORE_PATH "/root/my-release-key.keystore")
    
    # use regex to get the names of the app, package and artifact
    string (REGEX REPLACE "lib(.*).so" "\\1" APP_CUT ${target_name})
    set (ARTIFACT_NAME ${APP_CUT})
    string (REGEX REPLACE "-" " " APP_NAME ${APP_CUT})
    string (REGEX REPLACE "lib(.*).so" "com.\\1" PACKAGE_CUT ${target_name})
    string (REGEX REPLACE "-" "." PACKAGE ${PACKAGE_CUT})

    # might need to add the rsync command (cf build_android.sh:65)

    string (REGEX REPLACE "\\." "/" FORMATED_PACKAGE ${PACKAGE})
    string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD)
    set (ARTIFACT_PATH "${OUTPUT_PATH}/bin/${ARTIFACT_NAME}-${BUILD}.apk")

    # prepare resources
    add_custom_command (TARGET ${target}
        POST_BUILD
        COMMAND cp -r ${MINKO_HOME}/template/android/* ${OUTPUT_PATH}
        COMMAND mkdir -p "${OUTPUT_PATH}/src/${FORMATED_PACKAGE}"
        COMMAND mv "${OUTPUT_PATH}/src/*.java" "${OUTPUT_PATH}/src/${FORMATED_PACKAGE}"
        COMMAND sed -i 's/{{APP_NAME}}/${APP_NAME}/' ${OUTPUT_PATH}/res/values/strings.xml ${OUTPUT_PATH}/build.xml
        COMMAND sed -i 's/{{PACKAGE}}/${PACKAGE}/' ${OUTPUT_PATH}/AndroidManifest.xml ${OUTPUT_PATH}/src/${FORMATED_PACKAGE}/*.java
        COMMAND sed -i 's/{{VERSION_CODE}}/${VERSION}/' ${OUTPUT_PATH}/AndroidManifest.xml 
        COMMAND mkdir -p ${OUTPUT_PATH}/libs/armeabi-v7a/ && cp ${OUTPUT_PATH}/*.so ${OUTPUT_PATH}/libs/armeabi-v7a/ && mv ${OUTPUT_PATH}/libs/armeabi-v7a/${target_name} ${OUTPUT_PATH}/libs/armeabi-v7a/libmain.so
        COMMAND rm -rf ${OUTPUT_PATH}/assets
        COMMAND mv ${OUTPUT_PATH}/asset ${OUTPUT_PATH}/assets
        COMMAND chmod u+rwx -R ${OUTPUT_PATH}/assets
        COMMAND ant "${BUILD}"
        WORKING_DIRECTORY ${OUTPUT_PATH}
    )
    

    if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "release")
        set (UNSIGNED_APK_PATH "${OUTPUT_PATH}/bin/${APP_NAME}-${BUILD}-unsigned.apk")
        
        # sign the apk (only when making a release)
        add_custom_command (TARGET ${target}
            POST_BUILD
            COMMAND jarsigner -tsa "http://timestamp.digicert.com" -keystore ${ANDROID_KEYSTORE_PATH} -storepass ${ANDROID_KEYSTORE_PASSWORD} -verbose -sigalg SHA1withRSA -digestalg SHA1 ${UNSIGNED_APK_PATH} ${ANDROID_KEYSTORE_ALIAS}
            COMMAND jarsigner -verify -verbose -certs ${UNSIGNED_APK_PATH}
            COMMAND zipalign -fv 4 ${UNSIGNED_APK_PATH} ${ARTIFACT_PATH}
            COMMAND rm -f ${UNSIGNED_APK_PATH}
        )
    else ()
        set (UNSIGNED_APK_PATH "${OUTPUT_PATH}/bin/${APP_NAME}-${BUILD}.apk")
        add_custom_command (TARGET ${target}
            POST_BUILD
            COMMAND mv ${UNSIGNED_APK_PATH} ${ARTIFACT_PATH}
        )
    endif ()

endfunction()