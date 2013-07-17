#!/bin/sh

AR="emar"
ARGS=""

for arg in "$@"; do
	if [ $arg == "-rcs" ]; then
		ARGS="${ARGS} rk"
	else
		ARGS="${ARGS} $arg"
	fi
done

${AR} ${ARGS}