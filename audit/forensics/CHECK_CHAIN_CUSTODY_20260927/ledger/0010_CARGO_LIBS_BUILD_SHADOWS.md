# μ0010 — Cargo libraries and build-script shadows

- date: 2026-09-27
- parent_commit: 0b6f5cb9fc5fe9eca0b133c0328776d6df698dde
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: DEPENDENCY_AND_BUILD_SHADOW
- claim_allowed: false

## Cargo manifests observed

Seven Cargo.toml manifests exist in the audited tree:
- root `Cargo.toml`;
- `b3sum/Cargo.toml`;
- `c/blake3_c_rust_bindings/Cargo.toml`;
- `reference_impl/Cargo.toml`;
- `test_vectors/Cargo.toml`;
- `tools/compiler_version/Cargo.toml`;
- `tools/instruction_set_support/Cargo.toml`.

Three build scripts were observed:
- root `build.rs`;
- `c/blake3_c_rust_bindings/build.rs`;
- `tools/compiler_version/build.rs`.

## Lockfile asymmetry

Only one Cargo lockfile is tracked:
- `b3sum/Cargo.lock`.

No root `Cargo.lock` exists in the audited tree.

The tracked b3sum lock resolves 67 packages:
- 65 from crates.io registry with checksums;
- 2 local/path packages (`b3sum`, `blake3`).

Examples showing semver resolution beyond manifest lower bounds:
- manifest `anyhow = "1.0.25"` -> lock `1.0.104`;
- `arrayvec = "0.7.4"` -> `0.7.8`;
- `clap = "4.0.8"` -> `4.6.6`;
- `rayon-core = "1.12.1"` -> `1.13.0`;
- build dependency `cc = "1.1.12"` -> lock `1.4.3`.

For root-level Cargo commands, absence of a committed root lock means dependency resolution is not proven immutable by repository state alone.

Classification:
`ROOT_CARGO_RESOLUTION = MUTABLE_WITHIN_SEMVER_CONSTRAINTS`

This does not prove an unwanted dependency was selected. It proves the exact dependency graph is not fully encoded by the audited Git tree.

## Root build.rs shadow

A visible command such as:

`cargo build`

implicitly runs `build.rs`.

The root build script:
- reads `TARGET`, `HOST`, `CC`, `CFLAGS`, `CARGO_CFG_TARGET_*`, `CARGO_FEATURE_*`, `BLAKE3_CI`;
- invokes the external Rust crate `cc`;
- detects compiler feature support;
- compiles C and/or assembly implementations for SSE2/SSE4.1/AVX2/AVX512/NEON;
- mutates `CFLAGS` by appending `-fno-lto` when CFLAGS is present on non-MSVC builds;
- emits Cargo cfg selectors;
- deletes temporary .asm files in one Windows path.

Therefore:
`cargo build -> build.rs -> cc crate -> selected native compiler/assembler -> native source set`.

## C Rust-bindings build shadow

`c/blake3_c_rust_bindings/build.rs` additionally:
- accepts `BLAKE3_C_DIR_OVERRIDE`;
- uses the `ignore` crate to traverse files;
- optionally links system `tbb`;
- compiles different C/ASM source sets based on architecture/features.

`BLAKE3_C_DIR_OVERRIDE` is an environment-controlled source-root indirection. Current evidence does not show a workflow setting it outside the intended cross-test path, but the override surface exists.

## Compiler-version helper

`tools/compiler_version/build.rs` calls `cc::Build::get_compiler()` and exports the selected compiler path through `cargo::rustc-env`.

## Runner shadow

`.cargo/config.toml` defines:

`[target.wasm32-wasip1]`
`runner = "wasmtime"`

Thus `cargo test --target wasm32-wasip1` can cause an additional process execution through Wasmtime even though the Cargo command itself does not spell out the runtime.

## Dependency boundaries

Observed root dependency classes include:
- pure Rust crates;
- procedural macro path via derive features;
- memory mapping;
- threading;
- CPU feature detection;
- C compiler discovery/execution through `cc`;
- optional cryptographic/trait ecosystem crates used in tests/features.

No claim of malicious code inside any crate is made by this static inventory.

## Forensic classification

- explicit library dependency: OBSERVED
- transitive crates.io dependency: OBSERVED
- root exact dependency lock: ABSENT
- implicit build.rs execution: OBSERVED
- environment-directed compiler/source selection: OBSERVED
- hidden encrypted command payload: NOT_OBSERVED

## R3

- F_ok: Cargo libraries, lock boundary, build-script children, and runner shadow mapped.
- F_gap: GitHub Actions, containers, package managers, and remote installers remain.
- F_next: expand workflow supply-chain tails and mutable external references.
