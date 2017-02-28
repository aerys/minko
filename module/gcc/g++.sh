#!/bin/bash

BIN="${CXX:-g++}"
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

if [[ `uname -s` != "Darwin" || "${BIN}" != "g++" ]]; then
	# Terrible workaround because --start-group/--end-group options are not
	# supported on OS X (it's the default behavior).

	if [[ ! -z "${STATIC_LIBS}" || ! -z "${SHARED_LIBS}" ]]; then
		START_GROUP="-Wl,--start-group -Wl,--whole-archive"
		END_GROUP="-Wl,--no-whole-archive -Wl,--end-group"
	fi
fi

test "$verbose" != 0 && set -x

${BIN} ${ARGS} ${START_GROUP} ${STATIC_LIBS} ${SHARED_LIBS} ${END_GROUP}
