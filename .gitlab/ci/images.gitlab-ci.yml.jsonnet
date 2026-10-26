// Generates images.gitlab-ci.yml; edit this file, then regenerate and commit both:
//   jsonnet -S .gitlab/ci/images.gitlab-ci.yml.jsonnet > .gitlab/ci/images.gitlab-ci.yml
//
// The tag of an image is the value of its `tagArg` ARG in its Dockerfile,
// followed by `-<hash>`, a hash of its build context (images/<name>/): a changed
// image always gets a new tag, so two branches that change it never publish
// under the same tag. A pushed tag is never overwritten, so the digests pinned
// in image-refs.gitlab-ci.yml keep resolving.
//
// To bump an image:
// 1. Change its build context.
// 2. Run a pipeline on the branch from the web UI ("Run pipeline") with
//    IMAGE_PUSH_FORCE set to the image name: image:<name> builds, checks and
//    pushes it. IMAGE_BUILD_FORCE builds without pushing; to overwrite an
//    existing tag, which changes the digest it points to, also set
//    IMAGE_OVERWRITE to the image name.
// 3. Pin the tag and digest that image:<name> printed in image-refs.gitlab-ci.yml;
//    check:images-ci fails until the pinned tag is the current one. After a
//    rebase onto other changes to the image, its tag changes: push and pin it
//    again.
local images = [
  {
    name: 'android-build',
    tagArg: 'NDK_VERSION',
    tagPattern: 'r[0-9]+[a-z]?',
    // Runs in the built image, with the `tagArg` value in $IMAGE_BASE_TAG.
    check: 'test "$(basename "$ANDROID_NDK_HOME")" = "android-ndk-$IMAGE_BASE_TAG"'
           + ' && NDK_MAJOR=${IMAGE_BASE_TAG#r} && grep -E "^Pkg\\.Revision *= *${NDK_MAJOR%[a-z]}\\." "$ANDROID_NDK_HOME/source.properties"'
           + ' && cmake --version',
  },
];

local resolveImage(image) = image + {
  jobName: image.name,
  context: 'images/%s' % image.name,
  dockerfile: self.context + '/Dockerfile',
  // POSIX sh (the docker image has no bash); sets IMAGE_BASE_TAG, IMAGE_TAG and IMAGE.
  tagScript: |||
    IMAGE_BASE_TAG=$(sed -n -E 's/^ARG %(tagArg)s=(%(tagPattern)s)[[:space:]]*$/\1/p' %(dockerfile)s)
    if [ -z "$IMAGE_BASE_TAG" ] || [ "$(echo "$IMAGE_BASE_TAG" | wc -l)" -ne 1 ]; then
      echo "error: expected one 'ARG %(tagArg)s=<%(tagPattern)s>' in %(dockerfile)s" >&2
      exit 1
    fi
    # The paths and contents of the files of the build context.
    IMAGE_HASH=$(cd %(context)s && find . -type f | LC_ALL=C sort | xargs sha256sum | sha256sum | cut -c1-12)
    IMAGE_TAG="$IMAGE_BASE_TAG-$IMAGE_HASH"
    IMAGE="$CI_REGISTRY_IMAGE/images/%(name)s:$IMAGE_TAG"
    echo "Image: $IMAGE"
  ||| % self,
};

local resolvedImages = [resolveImage(image) for image in images];

local imageChanges(image) = [
  image.context + '/**/*',
  '.gitlab/ci/images.gitlab-ci.yml.jsonnet',
  '.gitlab/ci/images.gitlab-ci.yml',
];

// Builds and checks the image, then pushes it when a rule sets IMAGE_PUSH (one
// job, so the image is never saved and passed on as an artifact).
local imageJobs = [{
  ['image:' + image.jobName]: {
    extends: '.docker-in-docker',
    stage: 'build',
    needs: [],
    rules: [
      // Build and push if `IMAGE_PUSH_FORCE` is set to `"all"` or to the image
      // name, or on `main` when the image has changed
      {
        'if': '$IMAGE_PUSH_FORCE == "all" || $IMAGE_PUSH_FORCE == "' + image.name + '"',
        variables: { IMAGE_PUSH: 'true' },
      },
      {
        'if': '$CI_PIPELINE_SOURCE == "push" && $CI_COMMIT_REF_NAME == $CI_DEFAULT_BRANCH',
        changes: { paths: imageChanges(image) },
        variables: { IMAGE_PUSH: 'true' },
      },
      // Only build if `IMAGE_BUILD_FORCE` is set to `"all"` or to the image name,
      // or on a merge request when the image has changed compared to `main`
      { 'if': '$IMAGE_BUILD_FORCE == "all" || $IMAGE_BUILD_FORCE == "' + image.name + '"' },
      {
        'if': '$CI_PIPELINE_SOURCE == "merge_request_event"',
        changes: {
          paths: imageChanges(image),
          compare_to: 'refs/heads/main',
        },
      },
    ],
    script: |||
      # Credentials to pull the base image from registry.aerys.in.
      mkdir -p "$HOME/.docker" && echo "$DOCKER_AUTH_CONFIG" > "$HOME/.docker/config.json"
      # The runner pulls job images with DOCKER_AUTH_CONFIG, outside the script.
      # Docker CLI >= 28.3 would give it precedence over `docker login`; unset it
      # so the push keeps using the job token when the CLI of .docker-in-docker
      # is upgraded.
      unset DOCKER_AUTH_CONFIG
    ||| + image.tagScript + |||
      if [ "$IMAGE_PUSH" = true ]; then
        # The job token can read and write this project's registry. Its own
        # config keeps the credentials that pull the base image.
        PUSH_CONFIG="$PWD/.docker-push"
        echo "$CI_JOB_TOKEN" | docker --config "$PUSH_CONFIG" login -u gitlab-ci-token --password-stdin "$CI_REGISTRY"

        # Never overwrite a published tag (jobs pin it by digest) unless both
        # IMAGE_PUSH_FORCE and IMAGE_OVERWRITE name this image. The tag is a
        # hash of the build context, so an existing tag needs no rebuild.
        if { [ "$IMAGE_PUSH_FORCE" = all ] || [ "$IMAGE_PUSH_FORCE" = %(name)s ]; } \
            && { [ "$IMAGE_OVERWRITE" = all ] || [ "$IMAGE_OVERWRITE" = %(name)s ]; }; then
          echo "IMAGE_OVERWRITE=$IMAGE_OVERWRITE: pushing $IMAGE even if it exists"
        elif INSPECT=$(docker --config "$PUSH_CONFIG" manifest inspect "$IMAGE" 2>&1); then
          echo "$IMAGE already exists (this build context is published), not building it. Set IMAGE_OVERWRITE=%(name)s too to overwrite it."
          exit 0
        elif ! echo "$INSPECT" | grep -qiE 'no such manifest|manifest unknown'; then
          echo "error: cannot tell whether $IMAGE exists: $INSPECT" >&2
          exit 1
        fi
      fi

      docker build -t "$IMAGE" %(context)s
      docker run --rm -e IMAGE_BASE_TAG="$IMAGE_BASE_TAG" "$IMAGE" sh -c '%(check)s'

      if [ "$IMAGE_PUSH" != true ]; then
        echo "Not pushing $IMAGE (only on main, or with IMAGE_PUSH_FORCE)."
        exit 0
      fi

      docker --config "$PUSH_CONFIG" push "$IMAGE"
    ||| % image,
  },
} for image in resolvedImages];

// Checks that this file is up to date with its jsonnet source, and that
// image-refs.gitlab-ci.yml pins the current tag of each image.
local checkJob = {
  'check:images-ci': {
    stage: 'check',
    needs: [],
    image: 'alpine:3.22@sha256:2e0b81d60597f449baa56dc25d355367869d2dca4cd57e866f1e2b52af3eb55f',
    script: |||
      apk add --no-cache jsonnet
      jsonnet -S .gitlab/ci/images.gitlab-ci.yml.jsonnet | diff -u .gitlab/ci/images.gitlab-ci.yml -
    ||| + std.join('', [image.tagScript + |||
      PINNED=$(tr -d '\r' < .gitlab/ci/image-refs.gitlab-ci.yml | sed -n '/^\.image-%(name)s:/,/^$/s/^ *image: *//p')
      case "$PINNED" in
        "$IMAGE"@sha256:*) echo "Pinned: $PINNED" ;;
        *)
          echo "error: .image-%(name)s in image-refs.gitlab-ci.yml pins '$PINNED', not $IMAGE. Push the image (IMAGE_PUSH_FORCE=%(name)s), then pin its tag and digest." >&2
          exit 1
          ;;
      esac
    ||| % image for image in resolvedImages]),
  },
};

local jobs = imageJobs + [checkJob];

std.manifestYamlStream(
  [
    {
      variables: {
        IMAGE_BUILD_FORCE: {
          description: 'Build images even when there are no changes.',
          value: 'false',
          options: ['false', 'all'] + [image.name for image in images],
        },
        IMAGE_PUSH_FORCE: {
          description: 'Push images even when there are no changes (never overwrites an existing tag).',
          value: 'false',
          options: ['false', 'all'] + [image.name for image in images],
        },
        IMAGE_OVERWRITE: {
          description: 'Only with IMAGE_PUSH_FORCE naming the same image: overwrite an existing tag (changes the digest it points to).',
          value: 'false',
          options: ['false', 'all'] + [image.name for image in images],
        },
      },
    } + {
      [jobName]: job[jobName]
      for job in jobs
      for jobName in std.objectFields(job)
    },
  ],
  true,
  false,
  false,
)
