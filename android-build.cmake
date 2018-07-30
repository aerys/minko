function (build_android target target_name target_dir config)

    # define all the needed variables
    set (ANDROID_HOME "/opt/android-sdk-linux")
    set (ADB "${ANDROID_HOME}/platform-tools/adb")
    set (ZIPALIGN "${ANDROID_HOME}/build-tools/27.0.3/zipalign")
    set (VERSION "0")
    
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
    
    # replace what needs to be replaced
    execute_process(COMMAND
        sed -i 's/{{APP_NAME}}/${APP_NAME}/' res/values/strings.xml build.xml
    )
    execute_process(COMMAND
        sed -i 's/{{PACKAGE}}/${PACKAGE}/' AndroidManifest.xml src/${PACKAGE}/*.java
        # might need to add the // extension parameter
    )
    execute_process(COMMAND
        sed -i 's/{{VERSION_CODE}}/${VERSION_CODE}/' AndroidManifest.xml
    )

    # libs stuff
    execute_process(COMMAND 
        mkdir -p libs/armeabi-v7a/ && cp *.so libs/armeabi-v7a/ && mv libs/armeabi-v7a/${target_name} libs/armeabi-v7a/libmain.so
    )

endfunction()