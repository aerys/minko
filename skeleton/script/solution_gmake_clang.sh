#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
OS_TYPE=`uname -s`

pushd ${DIR}/.. > /dev/null
case ${OS_TYPE} in
	"Linux")
		${MINKO_HOME}/tool/lin/script/premake5.sh --cc=clang gmake
		;;
	"Darwin")
		${MINKO_HOME}/tool/lin/script/premake5.sh --cc=clang gmake
		;;
esac
popd > /dev/null
