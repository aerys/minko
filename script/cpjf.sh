#!/bin/bash

# The goal of this script is to copy all Java files from
# an absolute source folder to a relative destination directory
# keeping the hierarchy of each file

set -e

[[ $# -ne 2 ]] && {
    echo "usage: cpjf.sh absolute_src_dir relative_dest_dir" > /dev/stderr
    exit 1
}

ABSOLUTE_SRC_DIR=$1
RELATIVE_DEST_DIR=$2

[[ ! -d ${ABSOLUTE_SRC_DIR} ]] && {
    exit 0
}

CURRENT_DIR=$(pwd)

[[ $OSTYPE == "cygwin" ]] && {
    CURRENT_DIR=$(cygpath -w ${CURRENT_DIR})
}

# Move to the current plugin directory
pushd "${ABSOLUTE_SRC_DIR}" > /dev/null

JAVA_FILES=$(find . -iname '*.java')

for FILE in ${JAVA_FILES}
do
    DEST_DIR="${RELATIVE_DEST_DIR}/$(dirname ${FILE})"

    mkdir -p "${CURRENT_DIR}/${DEST_DIR}"

    cp "${ABSOLUTE_SRC_DIR}/${FILE}" "${CURRENT_DIR}/${DEST_DIR}"
done

popd > /dev/null
