# SmartShape Engine dev container

A hardened [Dev Container](https://containers.dev/) for Claude Code, opencode, and
generic IDE use (VS Code, JetBrains). It reuses the CI **GCC 9.4** toolchain image as
its base, so native `linux64` builds, tests, and `clang-format` run directly in the
container, and layers the **Docker CLI** on top so `script/build.sh <target> release`
— which shells out to `docker run` for each platform — works through the mounted host
Docker socket.

## Launching

### CLI (Claude Code / opencode / plain shell)

```bash
make dev-shell                                 # interactive bash
make dev-shell claude                          # Claude Code
make dev-shell opencode                        # opencode
make dev-shell script/build.sh linux64 release # build a target in-container
```

`make dev-shell` is a thin wrapper over `script/dev-shell.sh` (callable directly with
the same arguments if you prefer).

The recipe builds the image, maps your host UID, and mounts host config that exists
(`~/.gitconfig` read-only, `~/.config/glab-cli`, `~/.config/gh`, `~/.claude`,
`~/.claude.json`, opencode XDG dirs, the SSH agent socket, and the Docker socket).

### IDE

Open the folder in any Dev Container-aware IDE; it reads `devcontainer.json`. The IDE
remaps the `dev` user (UID 1000) to your host UID via `updateRemoteUserUID`; editors
that ignore `remoteUser` (e.g. Zed) start as root and the entrypoint drops to the
workspace owner UID via `gosu`.

Before the container starts, an `initializeCommand` pre-creates the host config
paths the `mounts` bind in (`~/.config/glab-cli`, `~/.config/gh`, `~/.claude`,
opencode dirs, and the `~/.gitconfig` / `~/.claude.json` files). This is required,
not cosmetic: devcontainer `mounts` use Docker's `--mount` syntax, which **fails
when the bind source is missing** (unlike `-v`), so without the pre-create a host
lacking one of those paths could not start the container — or Docker would
fabricate a root-owned directory in its place.

## What's included

- **Toolchain:** GCC 9.4, CMake, Make (from the base image), `clang-format`, `python3`
  + `pip` (for `test/websocket-server`), `git-lfs` configured system-wide.
- **Builds:** Docker CLI + Compose + Buildx. `script/build.sh` orchestrates the
  per-platform builds (android/html5/linux64/…) via the host Docker daemon.
- **AI tools:** Claude Code (native binary) and opencode.
- **Forge CLIs:** `glab` (GitLab) and `gh` (GitHub), both pre-authenticated from host
  config. `gh` keyring tokens are forwarded via `GH_TOKEN`; for `glab` on hosts that
  use an OAuth/keyring login, log in with a **PAT** so it round-trips into the
  container.
- **GPU:** Mesa Vulkan/GL userspace with software fallback (lavapipe/llvmpipe) — see
  below.

## GPU passthrough (best-effort)

The container always installs Mesa's software rasterizers, so a GPU app enumerates a
(software) Vulkan device even with no GPU on the host. Hardware acceleration is layered
on when `/dev/dri` is present.

- **CLI:** `script/dev-shell.sh` passes through `/dev/dri` device nodes automatically
  when they exist (and falls back to software rendering when they don't). No action
  needed.
- **IDE:** The Dev Container spec cannot mount a device conditionally, and
  `--device=/dev/dri` hard-fails on GPU-less hosts, so it is **not** in
  `devcontainer.json` by default (IDE users get software rendering). To enable hardware
  acceleration on a GPU host, add this to `devcontainer.json` and rebuild:

  ```json
  "runArgs": ["--device=/dev/dri"]
  ```

  The entrypoint adds the running user to the host's DRM device group automatically.
  NVIDIA-proprietary GPUs additionally require the host NVIDIA Container Toolkit; Mesa
  covers Intel, AMD, and the software fallback.

Verify with `vulkaninfo` inside the container: a hardware device name (Intel ANV / AMD
RADV) means passthrough worked; `llvmpipe`/`lavapipe` means the software fallback is
active.

**Mesa version caveat (focal base).** The base image is Ubuntu 20.04, whose Mesa (~20.x)
predates recent GPUs — e.g. on an AMD Strix Halo / RDNA 3.5 host, hardware Vulkan fails
to initialize (`ERROR_INITIALIZATION_FAILED`) and OpenGL silently falls back to llvmpipe.
Hardware acceleration works for GPUs that focal-era Mesa supports (older Intel/AMD).
When the hardware ICD is present but unusable, force clean software Vulkan with:

```bash
VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/lvp_icd.x86_64.json vulkaninfo
```

OpenGL needs no override — it falls back to llvmpipe automatically.

## Security notes

- The base image is pinned by digest; versioned tools carry `# renovate:` annotations.
- `~/.gitconfig` is mounted read-only; the container cannot mutate host git config.
- NPM postinstall scripts are blocked and a 24h package-age delay is enforced
  (`NPM_CONFIG_IGNORE_SCRIPTS`, `NPM_CONFIG_MINIMUM_RELEASE_AGE`) for Claude Code MCP
  installs.
- No network firewall is configured (interactive/IDE use). If you later want a
  default-DROP autonomous mode, re-run the devcontainer skill and opt into the firewall
  phase.
