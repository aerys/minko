#!/bin/bash


set -e
set -x

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

#
#
# Patch the OpenSSL configuration file.
#
# This is necessary so that the the arm 32 bits version of
# OpenSSL gets compiled instead of the linux-x86_64 version.
# The problem here is that the OpenSSL configuration file
# does not deal with the case where OS=android. Therefore it
# will fallback to the linux-x86_64 version.
sed -i 's/and OS=="linux"//g' ./lib/node/deps/openssl/openssl_no_asm.gypi
sed -i 's/and OS=="linux"//g' ./lib/node/deps/openssl/openssl-cl_no_asm.gypi

#
#
# Generate a standalone toolchain:
TOOLCHAIN_PATH="${PWD}/my_toolchain" # Change this to the path where you want the toolchain to be created.
rm -rf $TOOLCHAIN_PATH
${ANDROID_NDK_HOME}/build/tools/make_standalone_toolchain.py --arch arm --api 21 --install-dir ${TOOLCHAIN_PATH}


# Patch the toolchain (!)
#
# The following #undef are missing at line 69:
#    #undef sa_sigaction;
#    #undef sa_restorer;
#    #undef sa_handler;
# signal_types=${TOOLCHAIN_PATH}/sysroot/usr/include/bits/signal_types.h
# ex -s -c '69i|#undef sa_sigaction;' -c x ${signal_types}
# ex -s -c '69i|#undef sa_restorer;' -c x ${signal_types}
# ex -s -c '69i|#undef sa_handler;' -c x ${signal_types}

#
#
# Now we are ready to configure and compile node.
#
export GYP_DEFINES="host_os=linux"
target_compiler="${TOOLCHAIN_PATH}/bin/clang"
target_compilerpp="${TOOLCHAIN_PATH}/bin/clang++"

pushd lib/node
export CC=${target_compiler}
export CXX=${target_compilerpp}

# Even the host intermediate binaries must be compiled in 32 bits.
export CC_host="clang -m32"
export CXX_host="clang++ -m32 -lpthread"

./configure \
    --prefix=../install \
    --dest-cpu=arm \
    --cross-compiling \
    --dest-os=android \
    --without-intl \
    --without-inspector \
    --without-snapshot \
    --openssl-no-asm \
    --shared

popd

make -j8 -C lib/node
cp ./lib/node/out/Release/lib.target/libnode.so lib/nodejs/lib/android/libnode.so
