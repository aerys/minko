#!/bin/bash

BIN="g++"
ARGS=""

STATIC_LIBS=""
SHARED_LIBS=""

for ARG in "$@"; do
	if [[ $ARG = *.a ]]; then
		STATIC_LIBS="${STATIC_LIBS} ${ARG}"
	elif [[ $ARG = -l* ]]; then
		SHARED_LIBS="${SHARED_LIBS} ${ARG}"
	else
		ARGS="${ARGS} ${ARG}"
	fi
done

if [[ `uname -s` != "Darwin" && ( ! -z "${STATIC_LIBS}" || ! -z "${SHARED_LIBS}" ) ]]; then
	 # Options --start-group/--end-group are not supported on OS X (because this is the default behavior)
	START_GROUP="-Wl,--start-group"
	END_GROUP="-Wl,--end-group"
fi

test "$verbose" != 0 && set -x

${BIN} ${ARGS} ${START_GROUP} ${STATIC_LIBS} ${SHARED_LIBS} ${END_GROUP}
