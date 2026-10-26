# Toolchain image used by `script/build.sh` to build the Android targets.
# Based on the image vendor/node builds libnode.so with, so both share one NDK
# (r29, at $ANDROID_NDK_HOME); it only adds what the engine build needs.
FROM registry.aerys.in/aerys/smartshape/vendor/node/images/android-build:r29@sha256:2e0bf4db2346805373e0712fdd5bb533db5ac35c5e6c8d719c24613e8ba05730

# The NDK of the base image; also the image tag, followed by a hash of this
# directory (see .gitlab/ci/images.gitlab-ci.yml.jsonnet). image:android-build
# checks that it matches the base image's NDK.
ARG NDK_VERSION=r29

RUN apt-get update && apt-get install -y --no-install-recommends cmake \
    && rm -rf /var/lib/apt/lists/*
