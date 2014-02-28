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

if [[ `uname -s` != "Darwin" ]]; then # Option not supported on OS X (because this is the default behavior)
	START_GROUP="-Wl,--start-group"
	END_GROUP="-Wl,--end-group"
fi

if [[ -z "${STATIC_LIBS}" && -z "${SHARED_LIBS}" ]]; then
	test "$verbose" != 0 && echo "${BIN} ${ARGS}"
	${BIN} ${ARGS}
else
	test "$verbose" != 0 && echo "${BIN} ${ARGS} -Wl,--start-group ${STATIC_LIBS} ${SHARED_LIBS} -Wl,--end-group"
	${BIN} ${ARGS} ${START_GROUP} ${STATIC_LIBS} ${SHARED_LIBS} ${END_GROUP}
fi
