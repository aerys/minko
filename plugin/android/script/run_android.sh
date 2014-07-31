#!/bin/bash

set -x
set -e

TARGET=$1
TARGET_NAME=$(basename $TARGET)
TARGET_DIR=$(dirname $TARGET)
CONFIG=$(basename $TARGET_DIR)
APP_NAME=$(sed -r 's/lib(.*).so/\1/;s/([A-Za-z])([A-Za-z]+)/\U\1\L\2/g;s/[^[:alpha:]]//g' <<< "${TARGET_NAME}")
PACKAGE=$(sed -r 's/lib(.*).so/\1/;s/-/\./g;s/(.*)/\L\1/' <<< "${TARGET_NAME}")

pushd $TARGET_DIR > /dev/null

adb shell am start -n ${PACKAGE}/${PACKAGE}.MinkoActivity

popd > /dev/null
