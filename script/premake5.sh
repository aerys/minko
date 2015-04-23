#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

MACHINE_TYPE=`uname -m`
OS_TYPE=`uname -s`

export OSTYPE=${OS_TYPE}

case ${OS_TYPE} in
    Linux)
        case ${MACHINE_TYPE} in
            x86_64)
                ${DIR}/../bin/linux64/premake5 $@
                ;;
            i686)
                ${DIR}/../bin/linux32/premake5 $@
                ;;
            *)
                echo "Unknown architecture ${MACHINE_TYPE}" > /dev/stderr
                exit 1
                ;;
        esac
        ;;

    Darwin)
        case ${MACHINE_TYPE} in
            x86_64)
                ${DIR}/../bin/osx64/premake5 $@
                ;;
            *)
                echo "Unknown architecture ${MACHINE_TYPE}" > /dev/stderr
                exit 1
                ;;
        esac
        ;;

    CYGWIN*)
        case ${MACHINE_TYPE} in
            x86_64)
                ${DIR}/../bin/windows64/premake5.exe $@
                ;;
            i686)
                ${DIR}/../bin/windows32/premake5.exe $@
                ;;
            *)
                echo "Unknown architecture ${MACHINE_TYPE}" > /dev/stderr
                exit 1
            ;;
        esac
        ;;

    *)
        echo "Unknown OS ${OS_TYPE}" > /dev/stderr
        exit 1
        ;;
esac
