# μ0018 — Cargo build scripts, proc-macros and source-root override

- date: 2026-09-27
- parent_commit: `2333f7a096906990464b62b4eb6842acb4ddbb8b`
- audited_tree: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- kind: RUST_COMPILE_TIME_EXECUTION_GRAPH
- completion_state: PARTIAL
- claim_allowed: false

## μ0_START — manifests and build entry points

Audited Cargo surfaces:

- root `Cargo.toml`;
- `b3sum/Cargo.toml` + `b3sum/Cargo.lock`;
- `c/blake3_c_rust_bindings/Cargo.toml`;
- `reference_impl/Cargo.toml`;
- `test_vectors/Cargo.toml`;
- `tools/compiler_version/Cargo.toml`;
- `tools/instruction_set_support/Cargo.toml`.

The audited tree contains exactly three `build.rs` files:

1. `build.rs`;
2. `c/blake3_c_rust_bindings/build.rs`;
3. `tools/compiler_version/build.rs`.

Only `b3sum/Cargo.lock` is committed in this tree. No root `Cargo.lock` is present.

Boundary:

`DEPENDENCY_DECLARATION != RESOLVED_PACKAGE != BUILD_SCRIPT_EXECUTION != RUNTIME_LINKAGE`

## μ1_RESOLVE — compile-time tails

### A. root BLAKE3 build.rs

The root crate declares:

`[build-dependencies] cc = "1.1.12"`

The build script executes target/environment selection and uses `cc::Build` to invoke the native toolchain for C/assembly implementation variants.

Observed environment selectors include:

- `CC`;
- `CFLAGS`;
- `HOST`;
- `TARGET`;
- Cargo target/feature environment variables;
- target-specific compiler variables through the `cc` crate.

The source set selected can include SSE2/SSE4.1/AVX2/AVX512 assembly/C and NEON C based on target/features/compiler support.

Therefore `cargo build` is a process graph, not a pure Rust parser/compiler edge.

### B. C Rust-bindings build.rs — source-root shadow

`c/blake3_c_rust_bindings/Cargo.toml` declares build dependencies:

- `cc = "1.0.48"`;
- `ignore = "0.4.23"`.

Its `build.rs` defines:

`c_dir_path(filename)`

and, when environment variable `BLAKE3_C_DIR_OVERRIDE` is present, returns:

`<BLAKE3_C_DIR_OVERRIDE>/<filename>`

instead of the normal repository-relative `../<filename>`.

It then compiles C/C++/assembly sources through `cc::Build`.

Forensic consequence:

`CARGO -> build.rs -> BLAKE3_C_DIR_OVERRIDE -> alternate local source root -> native compiler`

This is a material source-authority boundary.

Classification:

- alternate source-root capability: OBSERVED;
- activation in a specific historical run: TOKEN_VAZIO until environment receipt;
- source bytes outside audited Git tree: POSSIBLE_IF_OVERRIDE_SET;
- unauthorized source substitution: NOT_PROVEN.

The same build script also observes `CC`, target-specific CC variables and Cargo target/feature variables.

### C. compiler_version build.rs

`tools/compiler_version/build.rs` calls `cc::Build::get_compiler()` and exports the discovered compiler path into Rust compilation through:

`cargo::rustc-env=COMPILER_PATH=...`

This means compiler discovery becomes compiled program metadata.

### D. b3sum locked graph

`b3sum/Cargo.lock` contains 67 package records.

Observed local records:
- `b3sum 1.8.7`;
- path-linked `blake3 1.8.7`.

The remaining registry package records carry crates.io index source identifiers and checksums in the lock.

Examples in the resolved graph include:
- `cc 1.4.3`;
- `clap 4.6.6`;
- `clap_derive 4.6.4`;
- `proc-macro2 1.0.107`;
- `quote 1.0.47`;
- `syn 3.0.3`;
- `rayon-core 1.13.0`;
- `memmap2 0.9.11`;
- `duct 1.1.1`;
- `tempfile 3.27.0`.

Not all locked packages are necessarily compiled on every target/feature set.

### E. procedural-macro execution

`b3sum/Cargo.toml` enables `clap` feature `derive`.

The lock graph resolves:

`clap -> clap_derive -> {heck, proc-macro2, quote, syn}`.

External verification for exact locked version `clap_derive 4.6.4` shows `[lib] proc-macro = true`.

Rust Reference states that procedural macros execute code during compilation and have compiler-equivalent file/resource access boundaries.

References:
- `https://docs.rs/crate/clap_derive/4.6.4/source/Cargo.toml`
- `https://doc.rust-lang.org/reference/procedural-macros.html`

Thus code from a resolved dependency can execute during compilation even though it is not a runtime BLAKE3 function call.

### F. unlocked/root resolution boundary

The repository root has no committed `Cargo.lock`.

Root direct version requirements include, among others:

- `arrayvec = 0.7.4`;
- `constant_time_eq = 0.4.2`;
- `cfg-if = 1.0.0`;
- optional digest/memmap/rayon/serde/zeroize families;
- target `cpufeatures = 0.3.0`;
- build dependency `cc = 1.1.12`.

Therefore exact transitive package versions for a root `cargo test/build` are not fixed solely by the audited Git tree.

`SEMVER_REQUIREMENT != IMMUTABLE_PACKAGE_SET`

## μ2_EXECUTE_OBSERVE — shadow classification

Confirmed compile-time execution/selection surfaces:

1. repository `build.rs` code;
2. registry build-dependency code;
3. native compiler selected via environment/toolchain discovery;
4. procedural macro execution for derive-enabled dependencies;
5. target/feature conditional source selection;
6. C/ASM compilation through `cc`;
7. optional local source-root redirection through `BLAKE3_C_DIR_OVERRIDE`;
8. dependency resolution outside the repository where no lock exists.

No evidence in this grain establishes:
- encrypted Cargo commands;
- a malicious crate;
- historical use of `BLAKE3_C_DIR_OVERRIDE`;
- a compromised crates.io checksum;
- that every locked dependency executes in every job.

Strongest governance gap:

A receipt that records only the Git commit is insufficient to prove the exact native source/compiler/package execution graph when environment-driven source/tool overrides and unlocked resolution remain possible.

## μ3_CLOSE — route result and next grain

### Result

The Rust/Cargo layer contains both visible and transitive execution.

The most important source-authority edge is:

`BLAKE3_C_DIR_OVERRIDE -> C/ASM source path used by build.rs`.

The most important package-authority edge is:

`root Cargo.toml without root Cargo.lock -> registry resolution at build time`.

The most important compile-time-code edge is:

`derive feature -> proc-macro crate -> code executed by compiler`.

### Required future receipt fields

- Cargo version;
- registry/index identity;
- generated/resolved lock graph hash;
- package name/version/checksum for every activated crate;
- build-script list and hashes;
- proc-macro list and hashes;
- `CC/CFLAGS/HOST/TARGET`;
- target-specific CC variables;
- `BLAKE3_C_DIR_OVERRIDE` exact state;
- resolved compiler path/hash;
- selected C/ASM source paths + hashes.

### R3

- F_ok: Cargo/build.rs/proc-macro/source-root boundaries isolated.
- F_gap: per-run resolved root dependency graph and environment values remain TOKEN_VAZIO.
- F_next: map GitHub workflow tag/release privilege boundary and all environment crossings workflow -> script -> Cargo/CMake/runtime.
