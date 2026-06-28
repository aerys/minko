# SmartShape Engine dev container.
#
# Base image is reused from CI (the GCC 9.4 toolchain image the
# `test:linux64_offscreen` job runs in) so native linux64 builds, tests and
# clang-format work in-container without a nested container. The Docker CLI is
# layered on top so `script/build.sh <target> release` — which orchestrates the
# cross-platform builds via `docker run` — works through the mounted host socket.
#
# renovate: datasource=docker depName=registry.aerys.in/aerys/smartshape/vendor/gcc
FROM registry.aerys.in/aerys/smartshape/vendor/gcc@sha256:d4a63069d9b69ca4233eecd17638356d7e01aeb66f447db5b3e606a75f527887

# No WORKDIR — the project is mounted at its host-native absolute path for git
# worktree compatibility (.git files contain absolute paths).

# -- Environment (set early so tool installers use the right paths) -----------
ENV HOME=/tmp/home
# .local/bin → Claude Code; .opencode/bin → opencode
ENV PATH="${HOME}/.local/bin:${HOME}/.opencode/bin:${PATH}"
ENV GLAB_CONFIG_DIR=/tmp/glab-config
ENV GH_CONFIG_DIR=/tmp/gh-config

# -- NPM supply-chain hardening (Claude Code runs `npm install` for MCP servers)
# Block postinstall scripts and require 24h package age for supply-chain safety.
ENV NPM_CONFIG_IGNORE_SCRIPTS=true
ENV NPM_CONFIG_MINIMUM_RELEASE_AGE=1440

# -- System packages + GPU userspace ------------------------------------------
# The base image already ships git, git-lfs, cmake, gcc/g++, make, curl and
# python3. Add the bits it lacks: TLS roots + gnupg (needed for the Docker apt
# repo), clang-format (used by script/format.sh) and pip (used by the
# test/websocket-server harness).
#
# Mesa Vulkan/GL userspace is installed in the same layer — it pulls from the
# same focal repos, so a separate RUN would only re-fetch the identical apt
# indices for no cache benefit. mesa-vulkan-drivers ships the hardware ICDs
# (Intel ANV, AMD RADV) AND the lavapipe software rasterizer, so a container
# with no /dev/dri still enumerates a (software) Vulkan device and a GPU app can
# reach its render loop. The entrypoint injects the host DRM device GIDs at
# runtime; the opt-in runArgs / CLI launcher deliver the actual device nodes.
# vulkaninfo verifies the stack.
#
# git-lfs is configured system-wide so LFS filters work for any UID (the project
# stores *.a/*.so/*.lib/*.dll via LFS).
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        gnupg \
        openssh-client \
        clang-format \
        python3-pip \
        libvulkan1 \
        mesa-vulkan-drivers \
        libgl1-mesa-dri \
        libegl1 \
        vulkan-tools \
    && rm -rf /var/lib/apt/lists/* \
    && git lfs install --system

# -- Docker CLI + Compose plugin ----------------------------------------------
# CLI only — no daemon. The host daemon runs every build via the mounted socket.
# Targets the Ubuntu repo because the base image is Ubuntu 20.04 (focal).
RUN install -m 0755 -d /etc/apt/keyrings \
    && curl -fsSL https://download.docker.com/linux/ubuntu/gpg \
        | gpg --dearmor -o /etc/apt/keyrings/docker.gpg \
    && chmod a+r /etc/apt/keyrings/docker.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu $(. /etc/os-release && echo "$VERSION_CODENAME") stable" \
        > /etc/apt/sources.list.d/docker.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        docker-ce-cli \
        docker-compose-plugin \
        docker-buildx-plugin \
    && rm -rf /var/lib/apt/lists/*

# -- gosu (privilege drop for IDEs that ignore remoteUser, e.g. Zed) ----------
# renovate: datasource=github-releases depName=tianon/gosu
ARG GOSU_VERSION="1.19"
RUN arch="$(dpkg --print-architecture)" \
    && curl -fsSL "https://github.com/tianon/gosu/releases/download/${GOSU_VERSION}/gosu-${arch}" -o /usr/local/bin/gosu \
    && chmod +x /usr/local/bin/gosu \
    && gosu --version

# -- Claude Code (native binary, auto-updates) --------------------------------
# HOME is set above so the installer places the binary at $HOME/.local/bin/claude.
RUN curl -fsSL https://claude.ai/install.sh | bash \
    && claude --version

# -- opencode (AI coding assistant, auto-updates) -----------------------------
RUN curl -fsSL https://opencode.ai/install | bash \
    && opencode --version

# -- GitLab CLI (glab) --------------------------------------------------------
# renovate: datasource=gitlab-releases depName=gitlab-org/cli registryUrl=https://gitlab.com
ARG GLAB_VERSION="1.105.0"
RUN arch="$(dpkg --print-architecture)" \
    && curl -fsSL "https://gitlab.com/gitlab-org/cli/-/releases/v${GLAB_VERSION}/downloads/glab_${GLAB_VERSION}_linux_${arch}.deb" -o /tmp/glab.deb \
    && dpkg -i /tmp/glab.deb \
    && rm /tmp/glab.deb \
    && glab --version \
    && rm -rf /tmp/glab-config \
    && mkdir -m 1777 /tmp/glab-config

# -- GitHub CLI (gh) ----------------------------------------------------------
# renovate: datasource=github-releases depName=cli/cli
ARG GH_VERSION="2.95.0"
RUN arch="$(dpkg --print-architecture)" \
    && curl -fsSL "https://github.com/cli/cli/releases/download/v${GH_VERSION}/gh_${GH_VERSION}_linux_${arch}.deb" -o /tmp/gh.deb \
    && dpkg -i /tmp/gh.deb \
    && rm /tmp/gh.deb \
    && gh --version \
    && rm -rf /tmp/gh-config \
    && mkdir -m 1777 /tmp/gh-config

# -- Toolchain smoke check ----------------------------------------------------
# Fail the build loudly if a tool the dev shell advertises is missing from PATH.
# The base image supplies gcc; clang-format and the Docker CLI come from apt
# above (and have no install-time --version check of their own).
RUN gcc --version && clang-format --version && docker --version

# -- Git and SSH configuration ------------------------------------------------
# Wildcard — the mount path is unknown at build time.
RUN git config --system safe.directory '*'

# Populate known hosts (no interactive prompt). git.aerys.in is scanned on both
# port 22 and 443 — the project's git remote is ssh://git@git.aerys.in:443.
# github.com is included because Claude Code connects to GitHub for distribution.
# -T 15: the git.aerys.in:443 endpoint sends its SSH banner slowly; the default
# 5s keyscan timeout misses it. || true keeps a rare CI network miss from
# breaking the build (the port-22 + github keys on the line above still land).
RUN mkdir -p /etc/ssh \
    && { ssh-keyscan -T 15 -t ecdsa,rsa,ed25519 git.aerys.in github.com >> /etc/ssh/ssh_known_hosts 2>/dev/null || true; } \
    && { ssh-keyscan -T 15 -p 443 -t ecdsa,rsa,ed25519 git.aerys.in >> /etc/ssh/ssh_known_hosts 2>/dev/null || true; }

# -- Non-root user (for remoteUser + updateRemoteUserUID) ---------------------
# IDEs that support the devcontainer spec remap this UID to match the host.
# IDEs that don't (Zed) rely on the entrypoint to drop to the workspace owner
# UID via gosu.
RUN groupadd --gid 1000 dev \
    && useradd --uid 1000 --gid 1000 --no-create-home --home-dir /tmp/home --shell /bin/bash dev

# -- Permissions --------------------------------------------------------------
# Sticky-bit world-writable so any remapped/arbitrary UID can use HOME, and the
# entrypoint can inject a passwd entry and DRM/Docker device GIDs at runtime.
RUN chmod -R 1777 /tmp/home \
    && chmod 0666 /etc/passwd /etc/group

# -- Entrypoint (arbitrary UID + device GID injection) ------------------------
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["bash"]
