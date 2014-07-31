#!/bin/bash

DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
OS_TYPE=`uname -s`

pushd ${DIR}/.. > /dev/null
case ${OS_TYPE} in
	"Linux")
		${MINKO_HOME}/tools/lin/scripts/premake_gmake_clang.sh
		;;
	"Darwin")
		${MINKO_HOME}/tools/mac/scripts/premake_gmake_clang.sh
		;;
esac
popd > /dev/null
