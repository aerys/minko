#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR=$(realpath "${DIR}/..")

pushd ${DIR}/.. > /dev/null

# Make sure to be on the master branch
GIT_BRANCH=`git rev-parse --symbolic-full-name --abbrev-ref HEAD`

if [[ "${GIT_BRANCH}" != "master" ]]; then
    echo "error: releases must be done on the \"master\" branch"
    exit 2;
fi

if git status --porcelain | grep .; then
    echo "error: the repository state is not clean, please make sure you don't have pending changes before to release a new version"
    exit 2;
fi

# Retrieve current version from VERSION
CURRENT_VERSION=`cat VERSION`
MAJOR_VERSION=`echo $CURRENT_VERSION | cut -f1 -d.`
MINOR_VERSION=`echo $CURRENT_VERSION | cut -f2 -d.`

# Should we increment minor or major?
if [[ "$1" == "minor" ]]; then
    echo "Update minor version"
    MINOR_VERSION=$(($MINOR_VERSION + 1))
elif [[ "$1" == "major" ]]; then
    echo "Update major version"
    MAJOR_VERSION=$(($MAJOR_VERSION + 1))
    MINOR_VERSION="0"
else
    echo "Invalid argument: ./release.sh (minor|major)" >&2
    exit 1
fi

NEXT_VERSION="$MAJOR_VERSION.$MINOR_VERSION"

echo "Current version: $CURRENT_VERSION"
echo "Next version: $NEXT_VERSION"

# Update version into VERSION file
echo "${NEXT_VERSION}" > VERSION

# Commit the updated files above
git commit -m "Release version $NEXT_VERSION." VERSION
git diff HEAD^..HEAD

read -p "Are you sure you want to push these changes? (y|n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]; then
    # Tag the master branch using the current version
    git tag -a "$NEXT_VERSION" -m "Version $NEXT_VERSION"
    # Push changes on master branch
    git push origin master
    echo "Changes on master branch pushed"
    # Push tag
    git push origin "$NEXT_VERSION"
    echo "Tag pushed"

    # Create a branch to easily add fix from this version
    git checkout -b "version/$NEXT_VERSION"
    git push origin "version/$NEXT_VERSION"
    echo "Branch version/$NEXT_VERSION pushed"

    # Go back to master branch
    git checkout master
else
    echo "Revert changes"
    git reset --hard HEAD~1
fi

popd > /dev/null
