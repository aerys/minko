#!/usr/bin/env bash
# Launch an interactive dev container shell for SmartShape Engine.
#
#   script/dev-shell.sh            # interactive bash
#   script/dev-shell.sh claude     # run Claude Code
#   script/dev-shell.sh opencode   # run opencode
#   script/dev-shell.sh script/build.sh linux64 release   # build inside the container
#
# The container reuses the CI GCC toolchain and mounts the host Docker socket, so
# script/build.sh works in-container (it shells out to `docker run` for each
# target). Host AI/git/forge config is mounted when present.
set -euo pipefail

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${DIR}/.." && pwd)"
IMAGE="smartshape-engine-devcontainer"

docker build -t "${IMAGE}" "${ROOT_DIR}/.devcontainer"

tty_flag=$( [[ -t 0 ]] && echo "-it" || echo "-i" )

run_args=(
    --rm $tty_flag --init
    --user "$(id -u):$(id -g)"
    -v "${ROOT_DIR}:${ROOT_DIR}" -w "${ROOT_DIR}"
    -e DEVCONTAINER_WORKSPACE="${ROOT_DIR}"
    -e COLORTERM="${COLORTERM:-}"
)

# SSH agent (for git over SSH against git.aerys.in)
if [[ -n "${SSH_AUTH_SOCK:-}" && -S "${SSH_AUTH_SOCK}" ]]; then
    run_args+=(-v "${SSH_AUTH_SOCK}:/tmp/ssh-agent.sock" -e SSH_AUTH_SOCK=/tmp/ssh-agent.sock)
fi

# Git identity (read-only — never let the container mutate host git config)
[[ -f "$HOME/.gitconfig" ]] && run_args+=(-v "$HOME/.gitconfig:/tmp/home/.gitconfig:ro")

# Forge CLI config
[[ -d "$HOME/.config/glab-cli" ]] && run_args+=(-v "$HOME/.config/glab-cli:/tmp/glab-config")
[[ -d "$HOME/.config/gh" ]] && run_args+=(-v "$HOME/.config/gh:/tmp/gh-config")

# gh keeps its token in the host OS keyring, which does not cross the container
# boundary. Forward it via `gh auth token` using the pass-through form (-e
# GH_TOKEN with no value) so the secret stays out of the docker argv.
if command -v gh >/dev/null 2>&1; then
    GH_TOKEN="$(gh auth token 2>/dev/null || true)"
    export GH_TOKEN
    [[ -n "$GH_TOKEN" ]] && run_args+=(-e GH_TOKEN)
fi

# Claude Code config (dual mount — plugin manifests store absolute host paths)
[[ -d "$HOME/.claude" ]] && run_args+=(
    -v "$HOME/.claude:/tmp/home/.claude"
    -v "$HOME/.claude:$HOME/.claude"
)
[[ -f "$HOME/.claude.json" ]] && run_args+=(-v "$HOME/.claude.json:/tmp/home/.claude.json")

# opencode config (XDG paths, no dual mount needed)
[[ -d "$HOME/.config/opencode" ]] && run_args+=(-v "$HOME/.config/opencode:/tmp/home/.config/opencode")
[[ -d "$HOME/.local/share/opencode" ]] && run_args+=(-v "$HOME/.local/share/opencode:/tmp/home/.local/share/opencode")
[[ -d "$HOME/.cache/opencode" ]] && run_args+=(-v "$HOME/.cache/opencode:/tmp/home/.cache/opencode")

# Docker socket (conditional — script/build.sh needs it for cross-platform builds)
if [[ -S /var/run/docker.sock ]]; then
    run_args+=(-v /var/run/docker.sock:/var/run/docker.sock)
    run_args+=(--group-add "$(stat -c '%g' /var/run/docker.sock)")
fi

# Host Docker registry credentials (read-only). script/build.sh pulls private
# images from registry.aerys.in; the in-container docker client reads
# ~/.docker/config.json to authenticate the pull. Mount ONLY config.json — a
# read-only mount of the whole ~/.docker dir breaks buildx, which writes to
# ~/.docker/buildx.
[[ -f "$HOME/.docker/config.json" ]] && run_args+=(-v "$HOME/.docker/config.json:/tmp/home/.docker/config.json:ro")

# GPU passthrough (conditional — /dev/dri is absent on GPU-less hosts, which then
# fall back to lavapipe/llvmpipe software rendering)
if [[ -d /dev/dri ]]; then
    for node in /dev/dri/render* /dev/dri/card*; do
        [[ -e "$node" ]] || continue
        run_args+=(--device "$node")
        run_args+=(--group-add "$(stat -c '%g' "$node")")
    done
fi

# Recognisable container name — without --name, docker assigns a random two-word
# name (e.g. "brave_almeida"), leaving concurrent devcontainers unidentifiable in
# `docker ps`. Derive from the workspace dir; append a numeric suffix when taken
# (concurrent worktrees / repeat launches), since --name must be unique.
base_name="$(basename "${ROOT_DIR}")-devcontainer"
container_name="${base_name}"; n=2
while docker ps -a --format '{{.Names}}' 2>/dev/null | grep -qx "${container_name}"; do
    container_name="${base_name}-${n}"; n=$((n + 1))
done
run_args+=(--name "${container_name}")

# No args → interactive bash; otherwise run the passed command in-container.
exec docker run "${run_args[@]}" "${IMAGE}" "${@:-bash}"
