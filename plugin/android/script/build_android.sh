#!/bin/bash

set -x
set -e

#RSYNC_OPTIONS="--ignore-existing"
TARGETDIR=$1

# pwd
# echo $ANDROID_HOME

rsync -vr "${MINKO_HOME}/plugin/android/template/" .

mkdir -p libs/armeabi-v7a/
# mkdir -p libs/x86/

cp $TARGETDIR/*.so libs/armeabi-v7a/libmain.so
# cp $TARGETDIR/*.so libs/x86/libmain.so
tests -d assets || ln -s -f $TARGETDIR/asset assets

ant debug
# adb uninstall org.libsdl.app
adb install -r $TARGETDIR/SDLActivity-debug.apk
#adb devices | tail -n +2 | cut -sf 1 | xargs -I {} adb -s {} install -r $TARGETDIR/SDLActivity-debug.apk
