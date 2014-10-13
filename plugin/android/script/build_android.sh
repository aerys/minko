#!/bin/bash

set -x
set -e


if [ $OSTYPE == "cygwin" ]; then
	MINKO_HOME=`cygpath -u "${MINKO_HOME}"`
	ANDROID=`cygpath -u "${ANDROID_HOME}"`
fi

#RSYNC_OPTIONS="--ignore-existing"
TARGET=$1
TARGET_NAME=$(basename $TARGET)
TARGET_DIR=$(dirname $TARGET)
CONFIG=$(basename $TARGET_DIR)
APP_NAME=$(sed -r 's/libminko-(.*).so/minko.\1/;s/-/ /g;s/([A-Za-z])([A-Za-z]+)/\U\1\L\2/g;s/([0-9]+)//g;s/[^[:alpha:]]//g' <<< "${TARGET_NAME}")
PACKAGE=$(sed -r 's/libminko-(.*).so/minko.\1/;s/-/\./g;s/\.([0-9]+)//g;s/(.*)/\L\1/' <<< "${TARGET_NAME}")

pushd $TARGET_DIR > /dev/null

rm -rf src
rsync -vr "${MINKO_HOME}/plugin/android/template/" .
mkdir -p src/${PACKAGE//.//}
mv src/MinkoActivity.java src/${PACKAGE//.//}

sed -i "s/{{APP_NAME}}/${APP_NAME}/" res/values/strings.xml build.xml
sed -i "s/{{PACKAGE}}/${PACKAGE}/" AndroidManifest.xml src/${PACKAGE//.//}/MinkoActivity.java

mkdir -p libs/armeabi-v7a/
# mkdir -p libs/x86/

cp *.so libs/armeabi-v7a/libmain.so
# cp *.so libs/x86/libmain.so

rm -rf assets
mv asset assets

ant $CONFIG
# adb uninstall org.libsdl.app
$ANDROID/platform-tools/adb install -r bin/$APP_NAME-$CONFIG.apk
#adb devices | tail -n +2 | cut -sf 1 | xargs -I {} adb -s {} install -r $TARGET_NAME-$CONFIG.apk

popd > /dev/null
