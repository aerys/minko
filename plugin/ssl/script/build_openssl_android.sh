#!/bin/bash -e
#@author Aleksandar Gotev (alex.gotev@mobimesh.it)
#Hints and code taken also from http://stackoverflow.com/questions/11929773/compiling-the-latest-openssl-for-android

if [ "$#" -ne 5 ]
then
    echo "Usage:"
    echo "./build_openssl_android <ANDROID_NDK_VERSION> <OPENSSL_VERSION> <ANDROID_TARGET_API> \\"
    echo "                <ANDROID_TARGET_ABI> <OUTPUT_PATH>"
    echo
    echo "Supported target ABIs: armeabi, armeabi-v7a, x86, x86_64, arm64-v8a"
    echo
    echo "Example using NDK r25b, OpenSSL 1.1.1s and Android API 14 for armeabi-v7a."
    echo "./build_openssl_android r25b \\"
    echo "                1.1.1s \\"
    echo "                14 \\"
    echo "                armeabi-v7a \\"
    echo "                /home/user/output/armeabi-v7a"
    exit 1
fi

ANDROID_NDK_VERSION=$1
OPENSSL_VERSION=$2
OPENSSL_TARGET_API=$3
OPENSSL_TARGET_ABI=$4
OPENSSL_OUTPUT_PATH=$5
ANDROID_NDK_TMP_FOLDER="/tmp/android-ndk"
OPENSSL_TMP_FOLDER="/tmp/openssl"
NDK_MAKE_TOOLCHAIN="$ANDROID_NDK_TMP_FOLDER/build/tools/make_standalone_toolchain.py"

rm -rf "$ANDROID_NDK_TMP_FOLDER"
mkdir -p "$ANDROID_NDK_TMP_FOLDER"
rm -rf "$OPENSSL_TMP_FOLDER"
mkdir -p "$OPENSSL_TMP_FOLDER"

echo "Downloading Android NDK ${ANDROID_NDK_VERSION}"
wget -O android-ndk.zip https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux.zip && \
	unzip android-ndk.zip && \
	rm -f android-ndk.zip && \
	mv android-ndk-${ANDROID_NDK_VERSION}/* ${ANDROID_NDK_TMP_FOLDER}

echo "Downloading OpenSSL ${OPENSSL_VERSION}"
OPENSSL_SHORT_VERSION="$(echo "$OPENSSL_VERSION" | sed s'/[a-z]$//' )"
curl -fLO "https://www.openssl.org/source/openssl-$OPENSSL_VERSION.tar.gz" || curl -fLO "https://www.openssl.org/source/old/$short_version/openssl-$OPENSSL_VERSION.tar.gz" && \
    tar xvzf openssl-$OPENSSL_VERSION.tar.gz && \
	rm -f openssl-$OPENSSL_VERSION.tar.gz && \
	mv openssl-$OPENSSL_VERSION/* ${OPENSSL_TMP_FOLDER}

function build_library {
    mkdir -p ${OPENSSL_OUTPUT_PATH}
    export PATH=$TOOLCHAIN_PATH:$PATH
    make -j8 CALC_VERSIONS="SHLIB_COMPAT=; SHLIB_SOVER=" all
    cp ${OPENSSL_TMP_FOLDER}/*.so ${OPENSSL_TMP_FOLDER}/*.a ${OPENSSL_OUTPUT_PATH}/.
    echo "Build completed! Check output libraries in ${OPENSSL_OUTPUT_PATH}"
}

if [ "$OPENSSL_TARGET_ABI" == "armeabi-v7a" ]
then
    ${NDK_MAKE_TOOLCHAIN} --api=${OPENSSL_TARGET_API} \
                          --install-dir="${OPENSSL_TMP_FOLDER}/android-toolchain-arm" \
                          --arch=arm
    export TOOLCHAIN_PATH="${OPENSSL_TMP_FOLDER}/android-toolchain-arm/bin"
    export TOOL=arm-linux-androideabi
    export NDK_TOOLCHAIN_BASENAME=${TOOLCHAIN_PATH}/${TOOL}
    export CC=$NDK_TOOLCHAIN_BASENAME-gcc
    export CXX=$NDK_TOOLCHAIN_BASENAME-g++
    export LINK=${CXX}
    export LD=$NDK_TOOLCHAIN_BASENAME-ld
    export AR=$NDK_TOOLCHAIN_BASENAME-ar
    export RANLIB=$NDK_TOOLCHAIN_BASENAME-ranlib
    export STRIP=$NDK_TOOLCHAIN_BASENAME-strip
    export ARCH_FLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16"
    export ARCH_LINK="-march=armv7-a -Wl,--fix-cortex-a8"
    export CPPFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export CXXFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -frtti -fexceptions "
    export CFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export LDFLAGS=" ${ARCH_LINK} "
    export ANDROID_NDK_HOME="/tmp/android-ndk/toolchains/llvm/prebuilt/linux-x86_64"
    echo "TOOLCHAIN_PATH=$TOOLCHAIN_PATH"
    echo "TOOL=$TOOL"
    echo "NDK_TOOLCHAIN_BASENAME=$NDK_TOOLCHAIN_BASENAME"
    echo "CC=$CC"
    cd ${OPENSSL_TMP_FOLDER}
    ./Configure android-armeabi --openssldir=${OPENSSL_OUTPUT_PATH}
    build_library

elif [ "$OPENSSL_TARGET_ABI" == "arm64-v8a" ]
then
    ${NDK_MAKE_TOOLCHAIN} --platform=android-${OPENSSL_TARGET_API} \
                          --install-dir="${OPENSSL_TMP_FOLDER}/android-toolchain-arm64" \
                          --arch="arm64"
    export TOOLCHAIN_PATH="${OPENSSL_TMP_FOLDER}/android-toolchain-arm64/bin"
    export TOOL=aarch64-linux-android
    export NDK_TOOLCHAIN_BASENAME=${TOOLCHAIN_PATH}/${TOOL}
    export CC=$NDK_TOOLCHAIN_BASENAME-gcc
    export CXX=$NDK_TOOLCHAIN_BASENAME-g++
    export LINK=${CXX}
    export LD=$NDK_TOOLCHAIN_BASENAME-ld
    export AR=$NDK_TOOLCHAIN_BASENAME-ar
    export RANLIB=$NDK_TOOLCHAIN_BASENAME-ranlib
    export STRIP=$NDK_TOOLCHAIN_BASENAME-strip
    export ARCH_FLAGS=
    export ARCH_LINK=
    export CPPFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export CXXFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -frtti -fexceptions "
    export CFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export LDFLAGS=" ${ARCH_LINK} "
    cd ${OPENSSL_TMP_FOLDER}
    ./Configure android --openssldir=${OPENSSL_OUTPUT_PATH}
    build_library

elif [ "$OPENSSL_TARGET_ABI" == "armeabi" ]
then
    ${NDK_MAKE_TOOLCHAIN} --arch=arm \
                          --api ${OPENSSL_TARGET_API} \
                          --install-dir="${OPENSSL_TMP_FOLDER}/android-toolchain-arm"
    export TOOLCHAIN_PATH="${OPENSSL_TMP_FOLDER}/android-toolchain-arm/bin"
    export TOOL=arm-linux-androideabi
    export NDK_TOOLCHAIN_BASENAME=${TOOLCHAIN_PATH}/${TOOL}
    export CC=$NDK_TOOLCHAIN_BASENAME-clang
    export CXX=$NDK_TOOLCHAIN_BASENAME-clang++
    export LINK=${CXX}
    export LD=$NDK_TOOLCHAIN_BASENAME-ld
    export AR=$NDK_TOOLCHAIN_BASENAME-ar
    export RANLIB=$NDK_TOOLCHAIN_BASENAME-ranlib
    export STRIP=$NDK_TOOLCHAIN_BASENAME-strip
    export ARCH_FLAGS="-mthumb"
    export ARCH_LINK=
    export CPPFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export CXXFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -frtti -fexceptions "
    export CFLAGS=" ${ARCH_FLAGS} -D__ANDROID_API__=${OPENSSL_TARGET_API} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export LDFLAGS=" ${ARCH_LINK} "
    cd ${OPENSSL_TMP_FOLDER}
    ./Configure android --openssldir=${OPENSSL_OUTPUT_PATH}
    build_library

elif [ "$OPENSSL_TARGET_ABI" == "x86" ]
then
    ${NDK_MAKE_TOOLCHAIN} --platform=android-${OPENSSL_TARGET_API} \
                          --install-dir="${OPENSSL_TMP_FOLDER}/android-toolchain-x86" \
                          --arch=x86
    export TOOLCHAIN_PATH="${OPENSSL_TMP_FOLDER}/android-toolchain-x86/bin"
    export TOOL=i686-linux-android
    export NDK_TOOLCHAIN_BASENAME=${TOOLCHAIN_PATH}/${TOOL}
    export CC=$NDK_TOOLCHAIN_BASENAME-gcc
    export CXX=$NDK_TOOLCHAIN_BASENAME-g++
    export LINK=${CXX}
    export LD=$NDK_TOOLCHAIN_BASENAME-ld
    export AR=$NDK_TOOLCHAIN_BASENAME-ar
    export RANLIB=$NDK_TOOLCHAIN_BASENAME-ranlib
    export STRIP=$NDK_TOOLCHAIN_BASENAME-strip
    export ARCH_FLAGS="-march=i686 -msse3 -mstackrealign -mfpmath=sse"
    export ARCH_LINK=
    export CPPFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export CXXFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -frtti -fexceptions "
    export CFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export LDFLAGS=" ${ARCH_LINK} "
    cd ${OPENSSL_TMP_FOLDER}
    ./Configure android-x86 --openssldir=${OPENSSL_OUTPUT_PATH}
    build_library

elif [ "$OPENSSL_TARGET_ABI" == "x86_64" ]
then
    ${NDK_MAKE_TOOLCHAIN} --platform=android-${OPENSSL_TARGET_API} \
                          --install-dir="${OPENSSL_TMP_FOLDER}/android-toolchain-x86_64" \
                          --arch=x86_64
    export TOOLCHAIN_PATH="${OPENSSL_TMP_FOLDER}/android-toolchain-x86_64/bin"
    export TOOL=x86_64-linux-android
    export NDK_TOOLCHAIN_BASENAME=${TOOLCHAIN_PATH}/${TOOL}
    export CC=$NDK_TOOLCHAIN_BASENAME-gcc
    export CXX=$NDK_TOOLCHAIN_BASENAME-g++
    export LINK=${CXX}
    export LD=$NDK_TOOLCHAIN_BASENAME-ld
    export AR=$NDK_TOOLCHAIN_BASENAME-ar
    export RANLIB=$NDK_TOOLCHAIN_BASENAME-ranlib
    export STRIP=$NDK_TOOLCHAIN_BASENAME-strip
    export CPPFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export CXXFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -frtti -fexceptions "
    export CFLAGS=" ${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 "
    export LDFLAGS=" ${ARCH_LINK} "
    cd ${OPENSSL_TMP_FOLDER}
    ./Configure linux-x86_64 --openssldir=${OPENSSL_OUTPUT_PATH}
    build_library

else
    echo "Unsupported target ABI: $OPENSSL_TARGET_ABI"
    exit 1
fi
