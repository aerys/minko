#!/bin/bash

# set -x
set -e

TARGET_DIR="bin/android/release"
TARGET_NAME="libminko-plugin-nodejs-worker.a"

LIBS="$(find lib/node -name '*.a')"

TOOLCHAIN=${ANDROID_HOME}/toolchains/default
SUFFIX=arm-linux-androideabi
AR=${TOOLCHAIN}/bin/${SUFFIX}-ar

[[ -f ${TARGET_DIR}/${TARGET_NAME} ]] || {
    echo "Missing ${TARGET_NAME}" > /dev/stderr
    exit 1
}

# Need to extract and merge objects from separate directories to avoid archive member collision (like node.o or version.o).
for LIB in ${LIBS}; do
    LIB_NAME=$(basename ${LIB})
    mkdir -p ${TARGET_DIR}/tmp/${LIB_NAME}
    ${AR} t ${LIB} | xargs ${AR} rvs ${TARGET_DIR}/tmp/${LIB_NAME}/${LIB_NAME}
    pushd ${TARGET_DIR}/tmp/${LIB_NAME} > /dev/null
    ${AR} x ${LIB_NAME}
    rm ${LIB_NAME}
    popd > /dev/null
done

pushd ${TARGET_DIR} > /dev/null
${AR} qc ${TARGET_NAME} $(find tmp -name '*.o')
rm -rf tmp
popd > /dev/null
