#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR=$(realpath "${DIR}/..")

pushd $ROOT_DIR > /dev/null

# Error codes.
ERROR_INVALID_USAGE=1
ERROR_MISSING_REQUIRED_BIN=2

# Global variables.
GITLAB_CI_YML_PATH=".gitlab-ci.yml"
HTML5_DOCKER_TAG=$(cat $GITLAB_CI_YML_PATH | sed -n 's#.*registry.aerys.in/aerys/smartshape-docker/html5:\([0-9a-zA-Z\-\_]*\).*$#\1#p' | head -1)
ANDROID_DOCKER_TAG=$(cat $GITLAB_CI_YML_PATH | sed -n 's#.*registry.aerys.in/aerys/smartshape-docker/android:\([0-9a-zA-Z\-\_]*\).*$#\1#p' | head -1)
LINUX64_DOCKER_TAG=$(cat $GITLAB_CI_YML_PATH | sed -n 's#.*registry.aerys.in/aerys/smartshape-docker/linux64:\([0-9a-zA-Z\-\_]*\).*$#\1#p' | head -1)
MAKE_ARGS="${MAKE_ARGS:-'-j$(nproc)'}"

usage_and_exit() {
    echo "Usage: $0 <target> <build-type> [--cmake '<cmake-args>']" 1>&2
    echo "" 1>&2
    echo "<target>      The target platform of the build. Available targets are:" 1>&2
    echo "                  * android" 1>&2
    echo "                  * html5" 1>&2
    echo "                  * linux64" 1>&2
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

    exit ${ERROR_INVALID_USAGE};
}

show_notification() {
    if [ -x "$(command -v notify-send)" ];
    then
        notify-send "$1"
    fi
}

if [ "$OSTYPE" == "msys" ]
then
    export MSYS_NO_PATHCONV=1
    ADDITIONAL_DOCKER_ARGS=""
else
    ADDITIONAL_DOCKER_ARGS="-t -v /etc/group:/etc/group:ro -v /etc/passwd:/etc/passwd:ro -u $(id -u $USER):$(id -g $USER)"
fi


build_html5_release() {
    docker run -i --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        registry.aerys.in/aerys/smartshape-docker/html5:$HTML5_DOCKER_TAG \
        bash -c "
            mkdir -p build-html5-release && cd build-html5-release
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DCMAKE_TOOLCHAIN_FILE=/emsdk_portable/emscripten/sdk/cmake/Modules/Platform/Emscripten.cmake \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine html5 release"
}

build_html5_debug() {
    docker run -i --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        registry.aerys.in/aerys/smartshape-docker/html5:$HTML5_DOCKER_TAG \
        bash -c "
            mkdir -p build-html5-debug && cd build-html5-debug
            cmake .. \
                -DCMAKE_BUILD_TYPE=Debug \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DCMAKE_TOOLCHAIN_FILE=/emsdk_portable/emscripten/sdk/cmake/Modules/Platform/Emscripten.cmake \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine html5 debug"
}

build_linux64_release() {
    docker run -i --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        registry.aerys.in/aerys/smartshape-docker/linux64:$LINUX64_DOCKER_TAG \
        bash -c "
            mkdir -p build-linux64-release && cd build-linux64-release
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
    docker run -i --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        registry.aerys.in/aerys/smartshape-docker/linux64:$LINUX64_DOCKER_TAG \
        bash -c "
            mkdir -p build-linux64-debug && cd build-linux64-debug
            cmake .. \
                -DCMAKE_BUILD_TYPE=Debug \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine linux64 debug"
}

build_android_release() {
    docker run -i --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        registry.aerys.in/aerys/smartshape-docker/android:$ANDROID_DOCKER_TAG \
        bash -c "
            mkdir -p build-android-release && cd build-android-release
            cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_EXAMPLES=OFF \
            -DWITH_PLUGINS=ON \
            -DWITH_NODEJS_WORKER=ON \
                -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine android release"
}

build_android_debug() {
    docker run -i --rm \
        -v ${PWD}:${PWD} -w ${PWD} \
        $ADDITIONAL_DOCKER_ARGS \
        registry.aerys.in/aerys/smartshape-docker/android:$ANDROID_DOCKER_TAG \
        bash -c "
            mkdir -p build-android-debug && cd build-android-debug
            cmake .. \
                -DCMAKE_BUILD_TYPE=Debug \
                -DWITH_EXAMPLES=OFF \
                -DWITH_PLUGINS=ON \
                -DWITH_NODEJS_WORKER=ON \
                -DCMAKE_TOOLCHAIN_FILE=/opt/android-ndk-linux/build/cmake/android.toolchain.cmake \
                $CMAKE_ARGS
            make $MAKE_ARGS
        "

    show_notification "Build finished: smartshape-engine android debug"
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
    android|html5|linux64)
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
