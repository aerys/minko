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

if [[ -z "${STATIC_LIBS}" && -z "${SHARED_LIBS}" ]]; then
	${BIN} ${ARGS}
else
	${BIN} ${ARGS} "-Wl,--start-group" ${STATIC_LIBS} ${SHARED_LIBS} "-Wl,--end-group"
fi
