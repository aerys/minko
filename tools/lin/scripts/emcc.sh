#!/bin/bash

if [[ -z "${EMSCRIPTEN}" ]]; then
	echo "EMSCRIPTEN is not defined" > /dev/stderr
	exit 1
fi

YELLOW='1;33'
RED='1;31'

BIN="${EMSCRIPTEN}/emcc"
ARGS=()

for ARG in "$@"; do
	ARGS+=("${ARG}")
done

test "$verbose" != 0 && echo "${BIN} ${ARGS}"

python "${BIN}" "${ARGS[@]}" #2>&1 >/dev/null | GREP_COLOR="${YELLOW}" grep -E -i --color 'warning|$'

exit ${PIPESTATUS[0]}
