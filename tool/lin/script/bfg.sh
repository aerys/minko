#!/bin/bash

set -x
set -e

BFG_URL="http://repo1.maven.org/maven2/com/madgag/bfg/1.11.7/bfg-1.11.7.jar"
BFG_BIN=`basename ${BFG_URL}`
BFG="/tmp/${BFG_BIN}"

[[ -n "$1" ]] || {
	echo "usage: bfg.sh path/to/local/repo.git" 2> /dev/null
	exit 1
}

REPO=`readlink -f $1`
TMP="/tmp/bfg/`basename ${REPO}`"

[[ -e "${BFG}" ]] || {
	wget -O ${BFG} ${BFG_URL}
}

[[ -d "${REPO}/.git"  ]] || {
	echo "${REPO} is not a valid git repository"
	exit 1
}

rm -rf ${TMP}
mkdir -p ${TMP}
cd ${TMP}

git clone ${REPO} .

BRANCHES=`git branch -a -r | grep -v HEAD | cut -d/ -f2,3 | tr "\\n" "," | sed 's/,$//'`
# BRANCHES=master,dev

for BRANCH in ${BRANCHES//,/ }; do
	git checkout ${BRANCH}
done

git checkout master

java -jar ${BFG} --strip-blobs-bigger-than 100K --protect-blobs-from ${BRANCHES} .

git reflog expire --expire=now --all
git gc --prune=now --aggressive
