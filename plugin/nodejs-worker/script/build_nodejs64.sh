#!/bin/bash

[[ -f plugin.lua ]] || {
    echo "Script must be executed from the plugin root." > /dev/stderr
    exit 1
}

# Example: ANDROID_NDK_HOME=/home/sebcrozet/Android/Sdk/ndk-bundle
[[ -n ${ANDROID_NDK_HOME} ]] || {
    echo "Missing ANDROID_NDK_HOME environment variable." > /dev/stderr
    exit 1
}

#
#
# Download and extract node 10.
#
#
rm -rf lib/node

ANDROID_NDK=
NODE_VERSION="10.x"
wget https://github.com/nodejs/node/archive/v${NODE_VERSION}.zip -O node-${NODE_VERSION}.zip
unzip node-${NODE_VERSION}.zip -d lib
mv lib/node-${NODE_VERSION} lib/node
rm -f node-${NODE_VERSION}.zip

# getservbyport_r is not defined in Android NDK libc.
sed -i 's/.*HAVE_GETSERVBYPORT_R.*//g' lib/node/deps/cares/config/android/ares_config.h

# Set the output binary as libnode.so
sed -i 's/so.%s/so/g' lib/node/configure.py
sed -i 's/  shlib_suffix %= node_module_version//g' lib/node/configure.py

# remove linker '-pie' flag since we generate a shared
# library instead of an executable
sed -i "s/, '-pie'//g" lib/node/common.gypi
sed -i "s/-fPIE/-fPIC/g" lib/node/common.gypi
# Add missing -lpthread when the host is linux.
# Not doing this leads to linker errors.
sed -i "s/-lrt/-lrt -lpthread/g" lib/node/deps/v8/gypfiles/v8.gyp

### First, generate a 64-bits standalone toolchain:
TOOLCHAIN_PATH="${PWD}/my_toolchain" # Change this to the path where you want the toolchain to be created.
rm -rf $TOOLCHAIN_PATH
${ANDROID_NDK_HOME}/build/tools/make_standalone_toolchain.py --arch arm64 --api 28 --install-dir ${TOOLCHAIN_PATH}

export GYP_DEFINES="host_os=linux"
target_compiler="${TOOLCHAIN_PATH}/bin/clang"
target_compilerpp="${TOOLCHAIN_PATH}/bin/clang++"

pushd lib/node
export CC=${target_compiler}
export CXX=${target_compilerpp}
export CC_host="clang"
export CXX_host="clang++"

./configure \
    --prefix=../install \
    --dest-cpu=arm64 \
    --cross-compiling \
    --dest-os=android \
    --without-intl \
    --without-inspector \
    --without-snapshot \
    --openssl-no-asm \
    --shared

popd

make -j8 -C lib/node

# This will generate a ./lib/node/out/Release/lib.target/libnode.so.64 file.