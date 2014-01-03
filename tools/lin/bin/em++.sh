#!/bin/bash

if [[ -z "${EMSCRIPTEN_HOME}" ]]; then
	echo "EMSCRIPTEN_HOME is not defined" > /dev/stderr
	exit 1
fi

BIN="${EMSCRIPTEN_HOME}/em++"
ARGS=""

STATIC_LIBS=""
SHARED_LIBS=""

for ARG in "$@"; do
	if [ ${ARG} == "-s" ]; then # Option is not supported
		ARGS="${ARGS}"
	elif [ ${ARG} == "-O3" ]; then # -O3 is not recommended for emscripten
		ARGS="${ARGS}"
	elif [[ ${ARG} = *.a ]]; then
		STATIC_LIBS="${STATIC_LIBS} ${ARG}"		
	elif [[ ${ARG} = -l* ]]; then
		SHARED_LIBS="${SHARED_LIBS} ${ARG}"
	else
		ARGS="${ARGS} ${ARG}"
	fi
done

EXPECTED_ORDER="webgl sdl framework"

NEW_STATIC_LIBS=""

for ARG in ${STATIC_LIBS}; do
	for LIB in ${EXPECTED_ORDER}; do
		if [[ ${ARG} = *${LIB}.a ]]; then
			continue 2
		fi
	done
	NEW_STATIC_LIBS="${NEW_STATIC_LIBS} ${ARG}"
done

for LIB in ${EXPECTED_ORDER}; do
	for ARG in ${STATIC_LIBS}; do
		if [[ ${ARG} = *${LIB}.a ]]; then
			NEW_STATIC_LIBS="${NEW_STATIC_LIBS} ${ARG}"
			continue 2
		fi
	done
done

if [[ -z "${NEW_STATIC_LIBS}" && -z "${SHARED_LIBS}" ]]; then
	test "$verbose" != 0 && echo "${BIN} ${ARGS}"
	${BIN} ${ARGS}
else
	test "$verbose" != 0 && echo "${BIN} ${ARGS} ${NEW_STATIC_LIBS} ${SHARED_LIBS}"
	${BIN} ${ARGS} ${NEW_STATIC_LIBS} ${SHARED_LIBS}
fi
