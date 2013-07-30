#!/bin/bash

set -x

AR="ar"
ARGS=""

function extract()
{
	local LIB=`realpath $1`
	local TMPFILE=`mktemp -d /tmp/ar.XXXXXX` || exit 1
	pushd $TMPFILE > /dev/null
	ar -x $LIB || exit 1
	popd > /dev/null
	echo `ls $TMPFILE/*.o`
}

TARGET=""

for ARG in "$@"; do
	if [[ ${ARG} == *.a ]]; then
		if [ -n "${TARGET}" ]; then
			OBJS=`extract ${ARG}`
			ARGS="${ARGS} ${OBJS}"
		else
			TARGET="${ARG}"
			ARGS="${ARGS} ${ARG}"
		fi
	else
		ARGS="${ARGS} ${ARG}"
	fi
done

${AR} ${ARGS}
