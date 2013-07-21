#!/bin/bash

CXX="em++"
ARGS=""

for arg in "$@"; do
	if [ $arg == "-s" ]; then # Option is not supported
		ARGS="${ARGS}"
	elif [ $arg == "-O3" ]; then # -O3 is not recommended for emscripten
		ARGS="${ARGS}"
	else
		ARGS="${ARGS} $arg"
	fi
done

${CXX} ${ARGS}