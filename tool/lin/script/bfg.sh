#!/bin/bash

set -x
set -e

BFG_URL="http://repo1.maven.org/maven2/com/madgag/bfg/1.11.7/bfg-1.11.7.jar"
BFG_BIN=`basename ${BFG_URL}`
BFG="/tmp/${BFG_BIN}"
MINKO_TMP="/tmp/minko"

test -e ${BFG} || {
	wget -O ${BFG} ${BFG_URL}
}

test -n ${MINKO_HOME} || {
	echo "MINKO_HOME is not defined"
	exit 1
}

rm -rf ${MINKO_TMP}
mkdir -p ${MINKO_TMP}
cd ${MINKO_TMP}

git clone --mirror ${MINKO_HOME} .

BRANCHES=`git branch -a -r | grep -v HEAD | cut -d/ -f2,3 | tr "\\n" "," | sed 's/,$//'`

java -jar ${BFG} --strip-blobs-bigger-than 500K --protect-blobs-from ${BRANCHES} .

git reflog expire --expire=now --all
git gc --prune=now --aggressive
