#!/bin/bash

set -e

EMSDK_EMSCRIPTEN_VERSION="1.13.0"

EMSDK_WIKI="http://github.com/kripken/emscripten/wiki/Emscripten-SDK#wiki-downloads"
EMSDK_URL="https://s3.amazonaws.com/mozilla-games/emscripten/releases"
EMSDK_ARCHIVE="emsdk-${EMSDK_EMSCRIPTEN_VERSION}-portable-64bit.zip"
EMSDK_SDK="sdk-${EMSDK_EMSCRIPTEN_VERSION}-64bit"

if [[ `uname` == "Linux" ]]; then
	echo "Emscripten SDK is not officialy supported on Linux."
	read -p "Press any key to continue anyway... " -r -n 1
fi

if [[ -n "${EMSCRIPTEN}" ]]; then
	if [[ ! -x "${EMSCRIPTEN}/emcc" ]]; then
		echo "Environment variable EMSCRIPTEN is defined but does not contain an emcc executable."
		exit 1
	elif [[ -x "${EMSCRIPTEN}/../../emsdk" ]]; then
		EMSDK_PATH=`realpath "${EMSCRIPTEN}/../.."`
		echo "Updating the SDK installed in ${EMSDK_PATH}..."
		python "${EMSCRIPTEN}/../../emsdk" update
		exit 0
	else	
		echo "Environment variable EMSCRIPTEN is defined but this is not an Emscripten SDK."
		exit 1
	fi
fi

EMSDK_PATH="/usr/local/emsdk"
read -p "Where do you want to install the Emscripten SDK? [default: ${EMSDK_PATH}] " -r
echo

if [[ -n "${REPLY}" ]]; then
	EMSDK_PATH=${REPLY}
fi

read -p "Installing the Emscripten SDK to ${EMSDK_PATH}. Press any key to continue... " -r -n 1
echo

mkdir -p "${EMSDK_PATH}"

if [[ -n "$(ls -A ${EMSDK_PATH})" ]]; then
	echo "${EMSDK_PATH} is not empty or readable."
	exit 1
fi

curl -fSL "${EMSDK_URL}/${EMSDK_ARCHIVE}" -o "/tmp/${EMSDK_ARCHIVE}"
unzip -q -d "${EMSDK_PATH}" "/tmp/${EMSDK_ARCHIVE}"
python "${EMSDK_PATH}/emsdk" update
python "${EMSDK_PATH}/emsdk" install "${EMSDK_SDK}"
echo

echo "Success."
echo

echo "Please set the EMSCRIPTEN environment variable to ${EMSDK_PATH}/emscripten/${EMSDK_EMSCRIPTEN_VERSION}."
echo "You should run something like:"
echo
echo "    echo EMSCRIPTEN=${EMSDK_PATH}/emscripten/${EMSDK_EMSCRIPTEN_VERSION} >> ~/.profile"
echo
