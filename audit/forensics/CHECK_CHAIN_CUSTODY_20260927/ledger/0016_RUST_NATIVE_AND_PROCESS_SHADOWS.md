# μ0016 — Rust FFI, mmap, build-time and process shadows

- date: 2026-09-27
- parent_commit: 9815bd61cd6d0fc1fddcbbaded337f3b36e05c46
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: RUST_NATIVE_BOUNDARY_GRAPH
- rust_files_enumerated: 36
- claim_allowed: false

## Coverage

All 36 `.rs` files in the audited tree were enumerated for process spawning, environment-driven execution, FFI/native boundaries, unsafe code, memory mapping, compile-time include, dynamic-library loading, and network primitives.

## Process execution

Core library Rust code does not expose a general child-process launcher in this scan.

`tools/compiler_version/src/main.rs` does execute child processes:
- `Command::new(env!("CARGO"))` to run Cargo/rustc version commands;
- `Command::new(env!("COMPILER_PATH"))` to run the C compiler version command.

`COMPILER_PATH` is set at build time by `tools/compiler_version/build.rs`, which asks the external `cc` crate to detect a compiler.

Observed transitive chain:
`cargo build tool -> compiler_version/build.rs -> cc::Build::get_compiler -> cargo::rustc-env=COMPILER_PATH -> compiled tool -> Command::new(COMPILER_PATH)`.

This is an environment/toolchain resolution shadow with a repository-visible chain.

## Test command execution

`b3sum/tests/cli_tests.rs` uses the `duct` command macro to execute the built b3sum binary in CLI tests. The executable path is obtained from Cargo's `CARGO_BIN_EXE_b3sum` compile-time environment.

This is a test-only process boundary, not a network fetch.

## FFI/native backend boundary

Observed Rust modules declare or call `unsafe extern "C"` functions for BLAKE3 SIMD/native implementations, including SSE2, SSE4.1, AVX2, AVX512 and NEON paths.

The actual object code behind these FFI symbols is selected/compiled by Cargo `build.rs` and the C/ASM toolchain recorded in μ0010.

Therefore:
`Rust function call -> platform dispatch -> FFI symbol -> C/ASM object selected at build time`.

Unsafe/FFI is not itself evidence of hidden behavior. It is a boundary where the function-level implementation resides outside the immediate Rust source file.

## Memory-mapping boundary

`src/io.rs` and `src/lib.rs` use optional `memmap2` support.

When the `mmap` feature is enabled, file hashing may use OS-backed memory mappings; `b3sum` enables the root crate `mmap` and `rayon` features through its path dependency.

The mmap helper falls back to ordinary reads when mapping is unsuitable or fails.

This is OS/runtime behavior not visible from the top-level hash call alone, but it is explicit in source.

## Compile-time include

`test_vectors/src/lib.rs` embeds `../test_vectors.json` with `include_str!`.

The embedded bytes are repository-local at compile time; no external loader is involved.

## Build-script environment shadows

As recorded in μ0010, root and C-binding build scripts select native sources/toolchains using `TARGET`, `HOST`, `CC`, `CFLAGS`, Cargo feature variables and, for C bindings, `BLAKE3_C_DIR_OVERRIDE`.

These build scripts are Rust programs automatically executed by Cargo, so the visible command `cargo build/test` does not enumerate their native children.

## Negative Rust findings

This enumeration did not identify:
- HTTP/network client primitives in the Rust source;
- `TcpStream`/`UdpSocket` runtime networking;
- `libloading`, `dlopen`, or `dlsym` dynamic-library loaders;
- shell command strings;
- encoded command decoders;
- runtime `include!` from external paths.

## Forensic classification

- build-time native fan-out: OBSERVED
- Rust-to-C/ASM FFI: OBSERVED
- OS mmap path: OBSERVED
- toolchain executable discovery/child execution: OBSERVED
- runtime network downloader in Rust: NOT_OBSERVED
- dynamic shared-library loader in Rust: NOT_OBSERVED
- deliberate concealment: NOT_ESTABLISHED

## R3

- F_ok: all Rust files enumerated; process, FFI, mmap and build shadows mapped.
- F_gap: C/C++ native source still needs process/network/dlopen/syscall boundary scan.
- F_next: enumerate all C/C++ source for system/exec/popen/dlopen/socket/network APIs and external ABI calls.