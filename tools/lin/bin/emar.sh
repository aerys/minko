#!/bin/bash

if [[ -z "${EMSCRIPTEN_HOME}" ]]; then
	echo "EMSCRIPTEN_HOME is not defined" > /dev/stderr
	exit 1
fi

BIN="${EMSCRIPTEN_HOME}/emar"
ARGS=""

for arg in "$@"; do
	if [ $arg == "-rcs" ]; then
		ARGS="${ARGS} rk"
	else
		ARGS="${ARGS} $arg"
	fi
done

test "$verbose" != 0 && echo "${BIN} ${ARGS}"
${BIN} ${ARGS}
