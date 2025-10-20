#!/bin/bash
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR=$(realpath "${DIR}/..")

pushd $ROOT_DIR > /dev/null

# Error codes.
ERROR_INVALID_USAGE=1
ERROR_MISSING_REQUIRED_BIN=2

# Global variables.
HTML5_DOCKER_IMAGE=emscripten/emsdk:3.1.43
EMSCRIPTEN_SDK=/emsdk/upstream/emscripten
ANDROID_DOCKER_IMAGE=registry.aerys.in/aerys/infrastructure/vendor/android-ndk:r25b-2-linux-x86_64@sha256:270e29f73f61d4285c0572ac66591d3090f13dae546b7c7bf83047ee0b41bd56
# GCC 9.4.0
GCC_DOCKER_IMAGE=registry.aerys.in/aerys/smartshape/vendor/gcc@sha256:d4a63069d9b69ca4233eecd17638356d7e01aeb66f447db5b3e606a75f527887
MAKE_ARGS="${MAKE_ARGS:-'-j$(nproc)'}"

usage_and_exit() {
    echo "Usage: $0 <target> <build-type> [--cmake '<cmake-args>']" 1>&2
    echo "" 1>&2
    echo "<target>      The target platform of the build. Available targets are:" 1>&2
    echo "                  * android32" 1>&2
    echo "                  * android64" 1>&2
    echo "                  * html5" 1>&2
    echo "                  * linux64" 1>&2
    echo "                  * linux64_offscreen" 1>&2
    echo "                  * windows64" 1>&2
    echo "" 1>&2
    echo "<build-type>  The type of build to perform. Available types are:" 1>&2
    echo "                  * debug" 1>&2
    echo "                  * release" 1>&2
    echo "" 1>&2
    echo "ENVIRONMENT VARIABLES" 1>&2
    echo "              The following environment variables can customize the build:" 1>&2
    echo "" 1>&2
    echo "              MAKE_ARGS" 1>&2
    echo "                  Arguments to pass to the make program. Default value: -j$(nproc)." 1>&2
    echo "" 1>&2
    echo "              BUILD_DIR" 1>&2
    echo "                  Build directory. Default value: build-{target}-{build-type}." 1>&2

    exit ${ERROR_INVALID_USAGE};
}

show_notification() {
    if [ -x "$(command -v notify-send)" ];
    then
        notify-send "$1"
    fi
}

# Check which shell is being used.
if [ "$OSTYPE" == "msys" ]    # `msys` is an environment that provides `bash` on Windows. It's automatically installed with Git for Windows.
then
    export MSYS_NO_PATHCONV=1 # Prevent `msys` from converting paths to the DOS path format.
    ADDITIONAL_DOCKER_ARGS=""
else
    ADDITIONAL_DOCKER_ARGS="-t -v /etc/group:/etc/group:ro -v /etc/passwd:/etc/passwd:ro -u $(id -u $USER):$(id -g $USER)"
fi


build_html5_release() {
    BUILD_DIR="${BUILD_DIR:-'build-html5-release'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        -e EMSCRIPTEN=${EMSCRIPTEN_SDK} \
        $ADDITIONAL_DOCKER_ARGS \
        $HTML5_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            emcmake cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN_SDK}/cmake/Modules/Platform/Emscripten.cmake \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine html5 release"
}

build_html5_debug() {
    BUILD_DIR="${BUILD_DIR:-'build-html5-debug'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        -e EMSCRIPTEN=${EMSCRIPTEN_SDK} \
        $ADDITIONAL_DOCKER_ARGS \
        $HTML5_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            emcmake cmake .. \
                -DCMAKE_BUILD_TYPE=Debug \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN_SDK}/cmake/Modules/Platform/Emscripten.cmake \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine html5 debug"
}

build_linux64_release() {
    BUILD_DIR="${BUILD_DIR:-'build-linux64-release'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        $GCC_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine linux64 release"
}

build_linux64_debug() {
    BUILD_DIR="${BUILD_DIR:-'build-linux64-debug'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        $GCC_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            cmake .. \
                -DCMAKE_BUILD_TYPE=Debug \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine linux64 debug"
}

build_linux64_offscreen_release() {
    BUILD_DIR="${BUILD_DIR:-'build-linux64_offscreen-release'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        $GCC_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DWITH_OFFSCREEN=ON \
                -DWITH_TEXTURE_COMPRESSOR=ON \
                -DWITH_TESTS=ON \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine linux64_offscreen release"
}

build_linux64_offscreen_debug() {
    BUILD_DIR="${BUILD_DIR:-'build-linux64_offscreen-debug'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        $GCC_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            cmake .. \
                -DCMAKE_BUILD_TYPE=Debug \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DWITH_OFFSCREEN=ON \
                -DWITH_TEXTURE_COMPRESSOR=ON \
                -DWITH_TESTS=ON \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine linux64_offscreen debug"
}

build_android32_release() {
    BUILD_DIR="${BUILD_DIR:-'build-android32-release'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        $ANDROID_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DWITH_NODEJS_WORKER=ON \
                -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake \
                -DANDROID_ABI=armeabi-v7a \
                -DANDROID_PLATFORM=android-26 \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine android32 release"
}

build_android32_debug() {
    BUILD_DIR="${BUILD_DIR:-'build-android32-debug'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        $ANDROID_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            cmake .. \
                -DCMAKE_BUILD_TYPE=Debug \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DWITH_NODEJS_WORKER=ON \
                -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake \
                -DANDROID_ABI=armeabi-v7a \
                -DANDROID_PLATFORM=android-26 \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine android32 debug"
}

build_android64_release() {
    BUILD_DIR="${BUILD_DIR:-'build-android64-release'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        $ANDROID_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DWITH_NODEJS_WORKER=ON \
                -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake \
                -DANDROID_ABI=arm64-v8a \
                -DANDROID_PLATFORM=android-26 \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine android64 release"
}

build_android64_debug() {
    BUILD_DIR="${BUILD_DIR:-'build-android64-debug'}"

    docker run --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        $ANDROID_DOCKER_IMAGE \
        bash -c "
            mkdir -p $BUILD_DIR && cd $BUILD_DIR
            cmake .. \
                -DCMAKE_BUILD_TYPE=Debug \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DWITH_NODEJS_WORKER=ON \
                -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake \
                -DANDROID_ABI=arm64-v8a \
                -DANDROID_PLATFORM=android-26 \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine android64 debug"
}

build_windows64_release() {
    BUILD_DIR="${BUILD_DIR:-'build-windows64-release'}"

    mkdir $BUILD_DIR
    cd $BUILD_DIR
    cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DWITH_EXAMPLES=OFF -DWITH_PLUGINS=ON ..
    msbuild.exe Project.sln //property:Configuration=Release //property:Platform=x64 //m:4

    show_notification "Build finished: smartshape-engine windows64 release"
}

build_windows64_debug() {
    BUILD_DIR="${BUILD_DIR:-'build-windows64-debug'}"

    mkdir $BUILD_DIR
    cd $BUILD_DIR
    cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DWITH_EXAMPLES=OFF -DWITH_PLUGINS=ON ..
    msbuild.exe Project.sln //property:Configuration=Debug //property:Platform=x64 //m:4

    show_notification "Build finished: smartshape-engine windows64 debug"
}

# Decode arguments.
POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    --cmake)
    CMAKE_ARGS="$2"
    shift # Past argument.
    shift # Past value.
    ;;
    *)    # Unknown option.
    POSITIONAL+=("$1") # Save it in an array for later.
    shift # Past argument.
    ;;
esac
done
set -- "${POSITIONAL[@]}" # Restore positional parameters.

# Retrieve positional parameters.
TARGET=$1
BUILD_TYPE=$2

# Check mandatory parameters.
case $TARGET in
    android32|android64|html5|linux64|linux64_offscreen|windows64)
    ;;
    *)
    echo "Unknown target or no target specified. Aborting."
    usage_and_exit
    ;;
esac

case $BUILD_TYPE in
    debug|release)
    ;;
    *)
    echo "Unknown build type or no build type specified. Aborting."
    usage_and_exit
    ;;
esac

command -v docker >/dev/null 2>&1 || {
    echo >&2 "'docker' required but not installed. Aborting.";
    exit ${ERROR_MISSING_REQUIRED_BIN};
}

build_${TARGET}_${BUILD_TYPE}
