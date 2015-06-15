#!/bin/bash

TARGET=$1

if [[ -z "${TARGET}" ]]; then
	echo "usage: fail.sh <target>" 1>&2
	exit 1
fi

RED=$(tput setaf 1)
RESET=$(tput sgr0)

echo "${RED}Post-build command failed for project ${TARGET}${RESET}" > /dev/stderr

rm ${TARGET}

exit 1
