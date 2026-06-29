# SmartShape Engine — developer task runner.
#
# This Makefile is a thin entry point for developer workflows; the engine itself
# is built with CMake via script/build.sh.

.DEFAULT_GOAL := help

# Words after `dev-shell` are passed straight through to the launcher, so e.g.
# `make dev-shell claude` and `make dev-shell script/build.sh linux64 release`
# both work. They are filtered out of the goal list and declared as no-op phony
# targets below so make doesn't try to build them.
DEV_SHELL_ARGS := $(filter-out dev-shell,$(MAKECMDGOALS))

.PHONY: help
help:
	@echo "SmartShape Engine — make targets:"
	@echo "  make dev-shell                                  Launch an interactive dev container shell"
	@echo "  make dev-shell claude                           Run Claude Code in the dev container"
	@echo "  make dev-shell opencode                         Run opencode in the dev container"
	@echo "  make dev-shell script/build.sh linux64 release  Build a target inside the dev container"
	@echo ""
	@echo "See .devcontainer/README.md for details."

# Launch the hardened dev container (see .devcontainer/). Builds the image if
# needed, maps the host UID, and mounts host git/SSH/AI/forge config + the Docker
# socket so script/build.sh works in-container.
.PHONY: dev-shell
dev-shell:
	@script/dev-shell.sh $(DEV_SHELL_ARGS)

ifneq ($(DEV_SHELL_ARGS),)
# Swallow the pass-through words so they don't error as unknown targets.
.PHONY: $(DEV_SHELL_ARGS)
$(DEV_SHELL_ARGS):
	@:
endif
