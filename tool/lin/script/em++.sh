#!/bin/bash

if [[ -z "${EMSCRIPTEN}" ]]; then
	echo "EMSCRIPTEN is not defined" 1>&2
	exit 1
fi

BIN="${EMSCRIPTEN}/em++"

declare -a ARGS
declare -a STATIC_LIBS
declare -a SHARED_LIBS

for ARG in "$@"; do
	if [[ "${ARG}" = *.a ]]; then
		STATIC_LIBS+=("${ARG}")
	elif [[ "${ARG}" = -l* ]]; then
		SHARED_LIBS+=("${ARG}")
	elif [[ "${ARG}" != "-s" ]]; then
		ARGS+=("${ARG}")
	fi
done

#EXPECTED_ORDER=("webgl" "sdl" "framework")
EXPECTED_ORDER=("sdl" "webgl" "framework")

declare -a NEW_STATIC_LIBS

for ARG in "${STATIC_LIBS[@]}"; do
	for LIB in "${EXPECTED_ORDER[@]}"; do
		if [[ ${ARG} = *${LIB}.a ]]; then
			continue 2
		fi
	done
	NEW_STATIC_LIBS+=("${ARG}")
done

for LIB in "${EXPECTED_ORDER[@]}"; do
	for ARG in "${STATIC_LIBS[@]}"; do
		if [[ ${ARG} = *${LIB}.a ]]; then
			NEW_STATIC_LIBS+=("${ARG}")
			continue 2
		fi
	done
done

if [[ ${#NEW_STATIC_LIBS[0]} -eq 0 && ${#SHARED_LIBS[@]} -eq 0 ]]; then
	test "$verbose" != 0 && echo "${BIN} ${ARGS[@]}"
	python "${BIN}" "${ARGS[@]}"
else
	test "$verbose" != 0 && echo "${BIN} ${ARGS[@]} ${NEW_STATIC_LIBS[@]} ${SHARED_LIBS[@]}"
	python "${BIN}" "${ARGS[@]}" "${NEW_STATIC_LIBS[@]}" "${SHARED_LIBS[@]}"
fi
