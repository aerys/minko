#!/bin/bash

# Copyright (c) 2014 Aerys

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


# Install Emscripten from emsdk.
# Usage: ./install_emscripten.sh

# Please change EMSCRIPTEN_VERSION to install a specific version.

# Author: Warren Seine (warren.seine@aerys.in), Aerys, 2014.
# Project: Minko (minko.io).
# License: MIT.

set -e

EMSCRIPTEN_VERSION="1.21.0"

EMSDK_WIKI="=http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html"
EMSDK_URL="https://s3.amazonaws.com/mozilla-games/emscripten/releases"
EMSDK_ARCHIVE="emsdk-${EMSCRIPTEN_VERSION}-portable-64bit.zip"
EMSDK_SDK="sdk-${EMSCRIPTEN_VERSION}-64bit"

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

read -p "Installing the Emscripten SDK ${EMSCRIPTEN_VERSION} to ${EMSDK_PATH}. Press any key to continue... " -r -n 1
echo

EMSCRIPTEN=${EMSDK_PATH}/emscripten/${EMSCRIPTEN_VERSION}

mkdir -p "${EMSDK_PATH}"

if [[ -n "$(ls -A ${EMSDK_PATH})" ]]; then
	echo "${EMSDK_PATH} is not empty or readable."
	exit 1
fi

curl -fSL "${EMSDK_URL}/${EMSDK_ARCHIVE}" -o "/tmp/${EMSDK_ARCHIVE}"
unzip -q -d "${EMSDK_PATH}" "/tmp/${EMSDK_ARCHIVE}"
python "${EMSDK_PATH}/emsdk" update
python "${EMSDK_PATH}/emsdk" install "${EMSDK_SDK}"
python "${EMSDK_PATH}/emsdk" activate "${EMSDK_SDK}"
echo

read -p "The installer will now add the EMSCRIPTEN environment variable to ~/.profile. Proceed? [y/n] " -r
echo

if [ "${REPLY}" == "y" ]; then
	echo export EMSCRIPTEN=${EMSCRIPTEN} >> ~/.profile
else
	echo "Ok. Please set the EMSCRIPTEN environment variable to ${EMSCRIPTEN}."
	echo "You will need to run something like:"
	echo
	echo "    echo export EMSCRIPTEN=${EMSCRIPTEN} >> ~/.profile"
	echo
fi

echo "Success."
echo
