#!/bin/bash
# Dev container entrypoint.
#
# Resolves the running UID to a passwd entry (so git/ssh/whoami work under an
# arbitrary --user), grants access to the host Docker socket and any DRM render
# devices by matching their GIDs, and finally drops privileges to the target UID
# when started as root (IDEs like Zed that ignore remoteUser).
set -euo pipefail

# Resolve target UID — priority: explicit env var > workspace owner > current UID.
target_uid="${DEVCONTAINER_UID:-$(id -u)}"
target_gid="${DEVCONTAINER_GID:-$(id -g)}"

# If root without an explicit UID, infer from the workspace owner (handles Zed).
if [[ "$(id -u)" = "0" ]] && [[ -z "${DEVCONTAINER_UID:-}" ]]; then
    workspace="${DEVCONTAINER_WORKSPACE:-$(pwd)}"
    if [[ -d "$workspace" ]]; then
        target_uid="$(stat -c '%u' "$workspace")"
        target_gid="$(stat -c '%g' "$workspace")"
    fi
fi

# Resolve the target username, injecting a passwd entry under a unique name when
# the UID has none. The image ships a build-time `dev` at UID 1000, so a second
# `dev` entry would make the name-based `gosu` drop below resolve to UID 1000
# instead of the host UID — use a distinct name and `head -1`.
target_user="$(getent passwd "$target_uid" | cut -d: -f1 | head -1)"
if [[ -z "$target_user" ]]; then
    target_user="user"
    echo "${target_user}:x:${target_uid}:${target_gid}::${HOME}:/bin/bash" >> /etc/passwd
fi

# Ensure a group with the given GID exists (creating one named $2 if not) and
# add the target user to it. Used to mirror host device/socket GIDs inside.
grant_group() {  # $1 = gid, $2 = fallback group name
    getent group "$1" >/dev/null 2>&1 || echo "$2:x:$1:" >> /etc/group
    usermod -aG "$(getent group "$1" | cut -d: -f1)" "$target_user" 2>/dev/null || true
}

# -- Docker socket access (match host GID) ------------------------------------
# Only meaningful on the root (IDE) path; the CLI --user path uses --group-add.
if [[ -S /var/run/docker.sock ]] && [[ "$(id -u)" = "0" ]]; then
    grant_group "$(stat -c '%g' /var/run/docker.sock)" docker
fi

# -- GPU access (match host DRM device GIDs) ----------------------------------
# Best-effort: /dev/dri is absent on GPU-less hosts — the loop is a no-op and
# the container falls back to lavapipe/llvmpipe software rendering.
if [[ -d /dev/dri ]] && [[ "$(id -u)" = "0" ]]; then
    for node in /dev/dri/render* /dev/dri/card*; do
        [[ -e "$node" ]] || continue
        node_gid="$(stat -c '%g' "$node")"
        grant_group "$node_gid" "drm${node_gid}"
    done
fi

# Drop privileges if running as root with a non-root target. gosu <username>
# (not uid:gid) so initgroups() loads the supplementary device groups above.
if [[ "$(id -u)" = "0" ]] && [[ "${target_uid}" != "0" ]]; then
    exec gosu "${target_user}" "$@"
fi

exec "$@"
