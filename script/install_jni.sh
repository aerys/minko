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


# Install Android SDK/NDK.
# Usage: ./install_jni.sh

# Please change ANDROID_SDK_VERSION and ANDROID_NDK_VERSION to install a specific version.

# Author: Warren Seine (warren.seine@aerys.in), Aerys, 2014.
# Project: Minko (minko.io).
# License: MIT.

set -e

ANDROID_ARCH="arm"
ANDROID_API="21"
ANDROID_TOOLCHAIN="arm-linux-androideabi-4.9"

if [[ -z "${ANDROID_HOME}" ]]; then
	if [[ -z "${ANDROID}" ]]; then
		echo "Environment variable ANDROID_HOME is not defined." > /dev/stderr
		exit 1
	else
		ANDROID_HOME="${ANDROID}"
	fi
fi

if [[ ! -e "${ANDROID_HOME}/tools/lib/sdk-common.jar" ]]; then
	echo "Environment variable ANDROID_HOME is defined but does not look like an Android SDK." > /dev/stderr
	exit 1
elif [[ ! -d "${ANDROID_HOME}/ndk" ]]; then
	echo "Environment variable ANDROID_HOME is defined but does not contain an 'ndk' directory." > /dev/stderr
	exit 1
elif [[ ! -n "$(ls ${ANDROID_HOME}/ndk)" ]]; then
	echo "Environment variable ANDROID_HOME is defined but the 'ndk' directory is empty." > /dev/stderr
	exit 1
fi

ANDROID_NDK_VERSION=$(ls ${ANDROID_HOME}/ndk | tail -n1)
ANDROID_NDK_HOME="${ANDROID_HOME}/toolchains/${ANDROID_TOOLCHAIN}"

# Build a standalone toolchain
pushd "${ANDROID_HOME}/ndk/${ANDROID_NDK_VERSION}" > /dev/null
if [[ ! -x ${ANDROID_HOME}/ndk/${ANDROID_NDK_VERSION}/build/tools/make_standalone_toolchain.py ]]; then
	echo "Invalid NDK path: ${ANDROID_HOME}/ndk/${ANDROID_NDK_VERSION}" > /dev/stderr
	exit 1
fi
${ANDROID_HOME}/ndk/${ANDROID_NDK_VERSION}/build/tools/make_standalone_toolchain.py --arch ${ANDROID_ARCH} --api ${ANDROID_API} --install-dir ${ANDROID_NDK_HOME}
popd > /dev/null

# Link default NDK.
pushd "${ANDROID_HOME}/toolchains/" > /dev/null
ln -s ${ANDROID_TOOLCHAIN} default
popd > /dev/null
