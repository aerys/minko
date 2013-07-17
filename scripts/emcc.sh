#!/bin/sh

CC="emcc"
ARGS=""
EXT="bc"

set -x

if [ $# -gt 0 ]; then
	if [ "${1#*\.${EXT}}" == "${1}" ]; then
		cp ${1} ${1}.${EXT}
		ARGS="${ARGS} ${1}.${EXT}"
		shift
	fi
fi

while [ $# -gt 0 ]; do
	ARGS="${ARGS} ${1}"
	shift
done

${CC} ${ARGS}