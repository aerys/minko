#!/bin/bash

set -x
set -e

#RSYNC_OPTIONS="--ignore-existing"
TARGET=$1
TARGET_NAME=$(basename $TARGET)
TARGET_DIR=$(dirname $TARGET)
CONFIG=$(basename $TARGET_DIR)
APP_NAME=$(sed -r 's/lib(.*).so/\1/;s/([A-Za-z])([A-Za-z]+)/\U\1\L\2/g;s/[^[:alpha:]]//g' <<< "${TARGET_NAME}")

set
pwd
# echo $ANDROID_HOME

pushd $TARGET_DIR > /dev/null

rsync -vr "${MINKO_HOME}/plugin/android/template/" .
sed -i "s/{{APP_NAME}}/${APP_NAME}/" res/values/strings.xml build.xml

mkdir -p libs/armeabi-v7a/
# mkdir -p libs/x86/

cp *.so libs/armeabi-v7a/libmain.so
# cp *.so libs/x86/libmain.so

rm -rf assets
mv asset assets

ant $CONFIG
# adb uninstall org.libsdl.app
adb install -r $APP_NAME-$CONFIG.apk
#adb devices | tail -n +2 | cut -sf 1 | xargs -I {} adb -s {} install -r $TARGET_NAME-$CONFIG.apk

popd > /dev/null
