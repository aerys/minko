#!/bin/bash

set -x
set -e

ANDROID_KEYSTORE_PATH="${ANDROID_KEYSTORE_PATH}"
ANDROID_KEYSTORE_USERNAME="${ANDROID_KEYSTORE_USERNAME}"
ANDROID_KEYSTORE_PASSWORD="${ANDROID_KEYSTORE_PASSWORD}"

if [ $OSTYPE == "cygwin" ]; then
	MINKO_HOME=`cygpath -u "${MINKO_HOME}"`
	ANDROID=`cygpath -u "${ANDROID_HOME}"`
	ANDROID_KEYSTORE_PATH=`cygpath -u "${ANDROID_KEYSTORE_PATH}"`
fi

#RSYNC_OPTIONS="--ignore-existing"
TARGET=$1
TARGET_NAME=$(basename $TARGET)
TARGET_DIR=$(dirname $TARGET)
CONFIG=$(basename $TARGET_DIR)
APP_NAME=$(sed -r 's/lib(.*).so/\1/;s/-/ /g;s/([A-Za-z])([A-Za-z]+)/\U\1\L\2/g;s/([0-9]+)//g;s/[^[:alpha:]]//g' <<< "${TARGET_NAME}")
PACKAGE=$(sed -r 's/lib(.*).so/com.\1/;s/-/\./g;s/\.([0-9]+)//g;s/(.*)/\L\1/' <<< "${TARGET_NAME}")

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

if [ $CONFIG == "release" ]; then
	if [ !-f "$ANDROID_KEYSTORE_PATH/.keystore" ]; then
		echo "Can't find .keystore file at $ANDROID_KEYSTORE_PATH" 
	fi

	# Sign the app
	jarsigner -tsa http://timestamp.digicert.com -storepass $ANDROID_KEYSTORE_PASSWORD -verbose -sigalg SHA1withRSA \
	-digestalg SHA1 -signedjar bin/$APP_NAME-$CONFIG.apk bin/$APP_NAME-$CONFIG-unsigned.apk $ANDROID_KEYSTORE_USERNAME

	# Verify that the app is properly signed
	jarsigner -verify -verbose -certs bin/$APP_NAME-$CONFIG.apk
	# zipalign ensures that all uncompressed data starts with a particular byte alignment relative to the start of the file, 
	# which reduces the amount of RAM consumed by an app.
	#zipalign -v 4 bin/$APP_NAME-$CONFIG-unsigned.apk bin/$APP_NAME-$CONFIG.apk
	# Don't forget to uninstall the app to avoid INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES error
	$ANDROID/platform-tools/adb uninstall $PACKAGE
fi

$ANDROID/platform-tools/adb install -r bin/$APP_NAME-$CONFIG.apk
#adb devices | tail -n +2 | cut -sf 1 | xargs -I {} adb -s {} install -r $TARGET_NAME-$CONFIG.apk

popd > /dev/null
