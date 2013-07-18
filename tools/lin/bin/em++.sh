#!/bin/bash

CXX="em++"
ARGS=""

for arg in "$@"; do
	if [ $arg == "-s" ]; then
		ARGS="${ARGS}"
	else
		ARGS="${ARGS} $arg"
	fi
done

${CXX} ${ARGS}