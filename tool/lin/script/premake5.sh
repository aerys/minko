#!/bin/bash

BASEDIR=$(dirname $0)
MACHINE_TYPE=`uname -m`
OS_TYPE=`uname -s`

case ${OS_TYPE} in
	"Linux")
		${BASEDIR}/../../lin/bin/premake5.sh $@
		;;
	"Darwin")
		${BASEDIR}/../../mac/bin/premake5.sh $@
		;;
esac
