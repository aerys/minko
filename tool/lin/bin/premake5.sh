#!/bin/bash

MACHINE_TYPE=`uname -m`
$(dirname $0)/${MACHINE_TYPE}/premake5 $@
