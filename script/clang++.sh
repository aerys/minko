#!/bin/bash

BIN="$1"
ARGS=""

shift

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

test "$verbose" != 0 && set -x

${BIN} ${ARGS} ${STATIC_LIBS} ${STATIC_LIBS} ${SHARED_LIBS} ${SHARED_LIBS}
