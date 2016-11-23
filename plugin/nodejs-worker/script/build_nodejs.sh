#!/bin/bash

# set -x

while test $# -gt 0
do
    case "$1" in
        --use-prebuilt) USE_PREBUILT=1
            ;;
        --*) echo "bad option $1"
            ;;
        *) echo "argument $1"
            ;;
    esac
    shift
done

NODE_VERSION="6.x"

[[ -r lib/node/out/Release/obj.target/libnode.a ]] && {
    exit 0
}

[[ -f plugin.lua ]] || {
    echo "Script must be executed from the plugin root." > /dev/stderr
    exit 1
}

[[ -n ${ANDROID_HOME} ]] || {
    echo "Missing ANDROID_HOME environment variable." > /dev/stderr
    exit 1
}

[[ -r lib/node/src/node_version.h ]] || {
    rm -rf lib/node

    wget https://github.com/nodejs/node/archive/v${NODE_VERSION}.zip -O node-${NODE_VERSION}.zip
    unzip node-${NODE_VERSION}.zip -d lib
    mv lib/node-${NODE_VERSION} lib/node
    rm -f node-${NODE_VERSION}.zip

    # getservbyport_r is not defined in Android NDK libc.
    sed -i 's/.*HAVE_GETSERVBYPORT_R.*//g' lib/node/deps/cares/config/android/ares_config.h
}

[[ -z $USE_PREBUILT ]] || {
    echo "Using pre-built version."
    exit 0
}

PLUGIN_DIR="${PWD}"

pushd lib/node
if [ -z "$2" ]; then
    ARCH=arm
else
    ARCH="$2"
fi

CC_VER="4.9"
case ${ARCH} in
    arm)
        DEST_CPU="$ARCH"
        SUFFIX="$ARCH-linux-androideabi"
        TOOLCHAIN_NAME="$SUFFIX"
        ;;
    x86)
        DEST_CPU="ia32"
        SUFFIX="i686-linux-android"
        TOOLCHAIN_NAME="$ARCH"
        ;;
    x86_64)
        DEST_CPU="ia32"
        SUFFIX="$ARCH-linux-android"
        TOOLCHAIN_NAME="$ARCH"
        ;;
    *)
        echo "Unsupported architecture provided: $ARCH"
        exit 1
        ;;
esac

export TOOLCHAIN=${ANDROID_HOME}/toolchains/default
export PATH=${TOOLCHAIN}/bin:${PATH}
export AR=${TOOLCHAIN}/bin/${SUFFIX}-ar
export CC=${TOOLCHAIN}/bin/${SUFFIX}-gcc
export CXX=${TOOLCHAIN}/bin/${SUFFIX}-g++
export LINK=${TOOLCHAIN}/bin/${SUFFIX}-g++

GYP_DEFINES="target_arch=$ARCH"
GYP_DEFINES+=" v8_target_arch=$ARCH"
GYP_DEFINES+=" android_target_arch=$ARCH"
GYP_DEFINES+=" host_os=linux OS=android"
export GYP_DEFINES

./configure \
    --dest-cpu=${DEST_CPU} \
    --dest-os=android \
    --without-snapshot \
    --without-intl \
    --without-inspector \
    --shared-openssl \
    --shared-openssl-includes="${PLUGIN_DIR}/../ssl/lib/openssl/include" \
    --shared-openssl-libpath="${PLUGIN_DIR}/../ssl/lib/openssl/lib/android" \
    --enable-static
popd

make -j4 -C lib/node
