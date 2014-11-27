#!/bin/bash

# we save the project's current directory location
CWD=`pwd`

ANDROID_KEYSTORE_PATH="${ANDROID_KEYSTORE_PATH}"
ANDROID_KEYSTORE_ALIAS="${ANDROID_KEYSTORE_ALIAS}"
ANDROID_KEYSTORE_PASSWORD="${ANDROID_KEYSTORE_PASSWORD}"
ANDROID="${ANDROID_HOME}"

VERSION_CODE="1"

if ! [ -z "${BUILD_NUMBER}" ]; then
	VERSION_CODE="${BUILD_NUMBER}"
fi

if [ $OSTYPE == "cygwin" ]; then
	MINKO_HOME=`cygpath -u "${MINKO_HOME}"`
	ANDROID=`cygpath -u "${ANDROID_HOME}"`
fi

#RSYNC_OPTIONS="--ignore-existing"
TARGET=$1
TARGET_NAME=$(basename $TARGET)
TARGET_DIR=$(dirname $TARGET)
CONFIG=$(basename $TARGET_DIR)
APP_NAME=$(sed -r 's/lib(.*).so/\1/;s/-/ /g;s/([A-Za-z])([A-Za-z]+)/\U\1\L\2/g;s/([0-9]+)//g;s/[^[:alpha:]]\s//g' <<< "${TARGET_NAME}")
PACKAGE=$(sed -r 's/lib(.*).so/com.\1/;s/-/\./g;s/\.([0-9]+)//g;s/(.*)/\L\1/' <<< "${TARGET_NAME}")

pushd $TARGET_DIR > /dev/null

rm -rf src
rsync -vr "${MINKO_HOME}/plugin/android/template/" .
rsync -vr "$CWD/android/" .
mkdir -p src/${PACKAGE//.//}
mv src/MinkoActivity.java src/${PACKAGE//.//}
mv src/*.java src/${PACKAGE//.//}

sed -i "s/{{APP_NAME}}/${APP_NAME}/" res/values/strings.xml build.xml
sed -i "s/{{PACKAGE}}/${PACKAGE}/" AndroidManifest.xml src/${PACKAGE//.//}/*.java

# Update version info
sed -i "s/{{VERSION_CODE}}/$VERSION_CODE/" AndroidManifest.xml

mkdir -p libs/armeabi-v7a/
# mkdir -p libs/x86/

cp *.so libs/armeabi-v7a/libmain.so
# cp *.so libs/x86/libmain.so

rm -rf assets
mv asset assets

ant $CONFIG

DEVICE_STATE=$($ANDROID/platform-tools/adb get-state | sed 's/\r$//')

if [ $CONFIG == "release" ]; then
	# Sign the app
	jarsigner -tsa http://timestamp.digicert.com -keystore $ANDROID_KEYSTORE_PATH -storepass $ANDROID_KEYSTORE_PASSWORD -verbose \
	-sigalg SHA1withRSA -digestalg SHA1 "bin/$APP_NAME-$CONFIG-unsigned.apk" $ANDROID_KEYSTORE_ALIAS

	# Verify that the app is properly signed
	jarsigner -verify -verbose -certs "bin/$APP_NAME-$CONFIG-unsigned.apk"
	
	# zipalign ensures that all uncompressed data starts with a particular byte alignment relative to the start of the file, 
	# which reduces the amount of RAM consumed by an app.
	$ANDROID/tools/zipalign -v 4 "bin/$APP_NAME-$CONFIG-unsigned.apk" "bin/$APP_NAME-$CONFIG.apk"
	
	# Don't forget to uninstall the app to avoid INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES error
	if [ $DEVICE_STATE == "device" ]; then
		$ANDROID/platform-tools/adb uninstall $PACKAGE
	fi
fi

# we move the final binary release into a unique folder
mkdir -p "bin/artifacts"
mv "bin/$APP_NAME-$CONFIG.apk" "bin/artifacts/$APP_NAME-$CONFIG.apk"

if [ $DEVICE_STATE == "device" ]; then
	$ANDROID/platform-tools/adb install -r "bin/artifacts/$APP_NAME-$CONFIG.apk"
fi

#adb devices | tail -n +2 | cut -sf 1 | xargs -I {} adb -s {} install -r $TARGET_NAME-$CONFIG.apk

popd > /dev/null
