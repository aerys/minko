#!/bin/bash

BASEDIR=$(dirname $0)
MACHINE_TYPE=`uname -m`
OS_TYPE=`uname -s`

case ${OS_TYPE} in
	"Linux")
		${BASEDIR}/../../lin/bin/${MACHINE_TYPE}/premake5.sh $@
		;;
	"Darwin")
		${BASEDIR}/../../mac/bin/${MACHINE_TYPE}/premake5.sh $@
		;;
esac
