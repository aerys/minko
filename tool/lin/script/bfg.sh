#!/bin/bash

set -x
set -e

BFG_URL="http://repo1.maven.org/maven2/com/madgag/bfg/1.11.7/bfg-1.11.7.jar"
BFG_BIN=`basename ${BFG_URL}`
BFG="/tmp/${BFG_BIN}"

[[ -n "$1" ]] || {
    echo "usage: bfg.sh git://path/to/repo.git" 2> /dev/null
    exit 1
}

[[ -e "${BFG}" ]] || {
    wget -O ${BFG} ${BFG_URL}
}

REPO="$1"
TMP="/tmp/bfg/`basename ${REPO}`"

rm -rf ${TMP}
mkdir -p ${TMP}

pushd "${TMP}"
git clone ${REPO} .

BRANCHES=`git branch -a -r | grep -v HEAD | cut -d/ -f2,3 | tr "\\n" "," | sed 's/,$//'`
# BRANCHES=master,dev

for BRANCH in `echo ${BRANCHES} | sed "s/,/ /g"`; do
    git checkout ${BRANCH}
done

git checkout master

OLD_SIZE=`du -hs ${TMP} | cut -f1`

java -jar ${BFG} --strip-blobs-bigger-than 100K --protect-blobs-from ${BRANCHES} .

git reflog expire --expire=now --all
git gc --prune=now --aggressive

NEW_SIZE=`du -hs ${TMP} | cut -f1`

echo "Repository: ${REPO}"
echo "Old size: ${OLD_SIZE}"
echo "New size: ${NEW_SIZE}"
read -p "Push? [y/N] " -r
echo

if [[ "${REPLY}" != "y" ]]; then
    echo "Canceled. Repo still visible in ${TMP}."
    exit 1
fi

git push -f --all
popd
