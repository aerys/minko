#!/bin/bash

if [[ -z "${EMSCRIPTEN_HOME}" ]]; then
	echo "EMSCRIPTEN_HOME is not defined" > /dev/stderr
	exit 1
fi

BIN="${EMSCRIPTEN_HOME}/emcc"
ARGS="$@"

test "$verbose" != 0 && echo "${BIN} ${ARGS}"
${BIN} ${ARGS}
