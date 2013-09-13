#!/bin/bash

MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
  $(dirname $0)/x64/premake4 $@
else
  $(dirname $0)/x32/premake4 $@
fi
