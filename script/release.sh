#!/bin/bash

set -e

if ! [ -x "$(command -v git)" ]; then
  echo "error: git is not installed" >&2
  exit 1
fi

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR=$(realpath "${DIR}/..")

pushd $ROOT_DIR > /dev/null

# Make sure to be on the master branch
GIT_BRANCH=`git rev-parse --symbolic-full-name --abbrev-ref HEAD`

if git status --porcelain | grep .; then
    echo "error: the repository state is not clean, please make sure you don't have pending changes before to release a new version"
    exit 2;
fi

# Retrieve current version from VERSION
CURRENT_VERSION=`cat VERSION`
MAJOR_VERSION=`echo $CURRENT_VERSION | cut -f1 -d.`
MINOR_VERSION=`echo $CURRENT_VERSION | cut -f2 -d.`
PATCH_VERSION=`echo $CURRENT_VERSION | cut -f3 -d.`

# Should we increment minor or major?
if [[ "$1" == "patch" ]]; then
    echo "Update patch version"
    PATCH_VERSION=$(($PATCH_VERSION + 1))

    if [[ ! "${GIT_BRANCH}" == version/* ]]; then
        echo "error: a patch release can only happen on a version branch" >&2
        exit 2
    fi
else
    if [[ "$1" == "minor" ]]; then
        echo "Update minor version"
        MINOR_VERSION=$(($MINOR_VERSION + 1))
        PATCH_VERSION="0"
    elif [[ "$1" == "major" ]]; then
        echo "Update major version"
        MAJOR_VERSION=$(($MAJOR_VERSION + 1))
        MINOR_VERSION="0"
        PATCH_VERSION="0"
    else
        echo "error: invalid argument: ./release.sh (minor|major|patch)" >&2
        exit 1
    fi

    if [[ "${GIT_BRANCH}" != "master" ]]; then
        echo "error: minor and major releases must be done on the \"master\" branch"
        exit 2;
    fi
fi


NEXT_VERSION="$MAJOR_VERSION.$MINOR_VERSION.$PATCH_VERSION"

echo "Current version: $CURRENT_VERSION"
echo "Next version: $NEXT_VERSION"

# Update VERSION
echo -n $NEXT_VERSION > VERSION

# Commit the updated files above
git commit -m "Release version $NEXT_VERSION." VERSION
git diff HEAD^..HEAD

read -p "Are you sure you want to make a release with those changes? (y|n)" -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]; then
    # Tag the master branch using the current version
    git tag -a "$NEXT_VERSION" -m "Version $NEXT_VERSION"

    if [[ "$1" != "patch" ]]; then
        VERSION_BRANCH="version/$MAJOR_VERSION.$MINOR_VERSION"
        # Create a branch to easily add fix from this version
        git checkout -b $VERSION_BRANCH

        # Go back to master branch
        git checkout "$GIT_BRANCH"

        echo "Release branch/tag created and ready: run 'git push origin master $VERSION_BRANCH $NEXT_VERSION' to push the release."
    else
        echo "Release tag created and ready: run 'git push origin $GIT_BRANCH $NEXT_VERSION' to push the release."
    fi
else
    echo "Reverting changes"
    git reset --hard HEAD~1
fi

popd > /dev/null
