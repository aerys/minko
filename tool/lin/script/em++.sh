#!/bin/bash

if [[ -z "${EMSCRIPTEN}" ]]; then
	echo "EMSCRIPTEN is not defined" 1>&2
	exit 1
fi

BIN="${EMSCRIPTEN}/em++"

declare -a ARGS

for ARG in "$@"; do
	if [[ "${ARG}" = *.a ]]; then
		STATIC_LIBS+=("${ARG}")
	elif [[ "${ARG}" = -l* ]]; then
		SHARED_LIBS+=("${ARG}")
	elif [[ "${ARG}" != "-s" ]]; then
		ARGS+=("${ARG}")
	fi
done

if [[ ${#STATIC_LIBS[0]} -eq 0 && ${#SHARED_LIBS[@]} -eq 0 ]]; then
	test "$verbose" != 0 && echo "${BIN} ${ARGS[@]}"
	python "${BIN}" "${ARGS[@]}"
else
	test "$verbose" != 0 && echo "${BIN} ${ARGS[@]} ${STATIC_LIBS[@]} ${STATIC_LIBS[@]} ${SHARED_LIBS[@]} ${SHARED_LIBS[@]}"
	python "${BIN}" "${ARGS[@]}" "${STATIC_LIBS[@]}" "${STATIC_LIBS[@]}" "${SHARED_LIBS[@]}" "${SHARED_LIBS[@]}"
fi
