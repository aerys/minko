#!/bin/bash

if [[ -z "${EMSCRIPTEN}" ]]; then
	echo "EMSCRIPTEN is not defined" > /dev/stderr
	exit 1
fi

BIN="${EMSCRIPTEN}/emar"
ARGS=()

for ARG in "$@"; do
	if [[ "${ARG}" == "-rcs" ]]; then
		ARGS+=("rk")
	else
		ARGS+=("${ARG}")
	fi
done

test "$verbose" != 0 && echo "${BIN} ${ARGS}"
python "${BIN}" "${ARGS[@]}"
