# μ0009 — Low-level execution surface inventory

- date: 2026-09-27
- parent_commit: 3275caf7a4c0ae2aa525dcf0f8b40a1b488c2bb5
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: STATIC_SURFACE_INVENTORY
- claim_allowed: false

## Tree census

Recursive Git tree enumeration:
- total entries: 1082
- blobs/files: 757
- tree truncated: false

Most frequent file classes:
- Markdown: 324
- C: 90
- shell: 70
- Rust: 36
- Python: 35
- headers: 33
- text: 28
- assembly .S: 25
- JSON: 20
- GitHub YAML .yml: 14
- TOML: 8
- CMake fragments: 4
- YAML .yaml: 3
- make fragments: 2

Execution/build/config candidates identified by extension, conventional filename, or executable bit: 144.

## Explicit executable-bit files

29 files carry mode 100755, including:
- RMR build and audit scripts;
- cross-matrix builders;
- standalone/freestanding artifact auditors;
- upstream comparison scripts;
- GitHub release uploader;
- C/Rust cross-test scripts;
- header/audit utilities.

Executable bit is only one execution surface. Many .sh/.py/.rs files without mode 100755 are still invoked explicitly by interpreters from workflows.

## Hidden/implicit execution classes to audit

### LIB
Imported/linked dependencies:
- Cargo dependencies/build-dependencies/dev-dependencies;
- system packages installed by apt/brew;
- CMake find_package results;
- Docker/container images;
- GitHub Actions;
- compiler/linker/runtime tools.

### TAIL
Downstream execution after an apparently simple parent command:
- shell scripts called by workflows;
- sourced shell libraries;
- command substitutions;
- pipes;
- build generators;
- post-test/report gates;
- child scripts.

### SHADOW
Behavior not visible at the immediate call site:
- Cargo build.rs execution;
- .cargo runner configuration;
- environment-variable overrides;
- mutable action refs/tags;
- remote installer scripts;
- externally resolved package versions;
- CMake package discovery/fetch behavior;
- generated build commands.

## Important negative findings from the complete tree

- Git submodules (mode 160000): none observed.
- versioned Git hooks: none observed by path inventory.
- custom .github/actions directory: none observed.
- executable symlink chain: none observed.
- only symlinks observed are three b3sum license links.

These negative findings reduce, but do not eliminate, indirect execution risk.

## Forensic rule

A command is not considered fully described until every child execution edge and every externally resolved artifact reachable from it has been classified.

`VISIBLE_COMMAND != TRANSITIVE_EXECUTION`

## R3

- F_ok: complete low-level tree census recorded.
- F_gap: dependency/resolution edges not yet expanded.
- F_next: expand Cargo/build.rs and runtime runner shadows.
