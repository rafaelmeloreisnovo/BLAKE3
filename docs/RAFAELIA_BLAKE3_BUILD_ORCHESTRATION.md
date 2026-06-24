# RAFAELIA BLAKE3 — build orchestration audit

Status: documentation audit, not a cryptographic redesign claim.
Scope: `build.rs`, `Cargo.toml`, `c/CMakeLists.txt`, `c/blake3.h`, `c/blake3_impl.h`, `c/blake3.c`, `c/blake3_dispatch.c`, `c/blake3_tbb.cpp`, C/Rust binding build scripts, and CI.

## 1. Core statement

This fork should not be described as “a better BLAKE3 algorithm”. The precise technical claim is stronger and safer:

> The cryptographic primitive remains BLAKE3-compatible, while this fork documents and extends the build/dispatch/orchestration layer around flags, warnings, SIMD selection, optional TBB parallel recursion, Rust/C binding tests, and reproducible build gates.

That distinction matters. The value is not a simple average score. It lives in the separation between:

- cryptographic semantics: preserved;
- compiler contract: strengthened;
- build pipeline: made more observable;
- runtime dispatch: kept compatible;
- optional parallel layer: added behind gates;
- documentation: now claim-gated instead of promotional.

## 2. What is actually present in the code

### 2.1 Cargo release/bench profile

`Cargo.toml` sets release and bench profiles with:

```toml
lto = true
codegen-units = 1
```

Interpretation: the Rust build favors whole-program optimization and more stable code generation over maximal incremental build speed. This is a compiler-pipeline decision, not just an algorithmic one.

### 2.2 Feature gates

The Rust crate keeps public feature gates for `std`, `rayon`, `mmap`, `zeroize`, `traits-preview`, `neon`, `wasm32_simd`, and testing-only SIMD exclusion flags. The important engineering point is that the feature matrix makes behavior explicit rather than implicit.

### 2.3 `build.rs` as precompiler/compile-time router

The root `build.rs` behaves like an orchestration layer before the compiler proper:

- declares environment dependencies with `cargo:rerun-if-env-changed`;
- turns build-script warnings into hard failures under `BLAKE3_CI`;
- avoids broad environment rebuild noise with `emit_rerun_if_env_changed(false)`;
- checks compiler support for AVX-512 flags before selecting code paths;
- chooses assembly, intrinsics, NEON, Wasm SIMD, or pure Rust fallback according to target and feature gates;
- emits `cargo:rustc-check-cfg` for known internal cfg names, reducing accidental cfg drift with modern Rust.

This is close to the layer Rafael called “pré-compilador-compilador”: it is not only passing flags; it is deciding which translation units are allowed to exist in the build graph.

### 2.4 CMake SIMD and TBB gates

`c/CMakeLists.txt` contains explicit options:

```cmake
option(BLAKE3_USE_TBB "Enable oneTBB parallelism" OFF)
option(BLAKE3_FETCH_TBB "Allow fetching oneTBB from GitHub if not found on system" OFF)
```

It also defines compiler flag variables for SSE2, SSE4.1, AVX2, AVX-512, NEON, and a selectable `BLAKE3_SIMD_TYPE`.

The TBB path is optional. When enabled and found, it adds `blake3_tbb.cpp`, links `TBB::tbb`, and exposes `BLAKE3_USE_TBB`. When not found, it warns and disables the TBB path rather than making the core unusable.

### 2.5 Public C header contract

`c/blake3.h` currently includes `<stddef.h>` and `<stdint.h>`, defines visibility macros, and adds:

```c
#if defined(__GNUC__) || defined(__clang__)
#define BLAKE3_RESTRICT __restrict__
#else
#define BLAKE3_RESTRICT
#endif
```

The public API uses `BLAKE3_RESTRICT` on selected pointer parameters. This is a real compiler-contract signal: it tells GCC/Clang that some API pointers should be treated as non-aliasing when the caller respects the contract.

Important correction: documentation must not claim that `<stddef.h>` and `<stdint.h>` were removed from the public header, because the current file includes them.

### 2.6 Internal include order and dispatch locality

`c/blake3_impl.h` keeps low-level includes before `blake3.h`, then defines internal flags, `INLINE`, target architecture markers, NEON autodetection, SIMD degree, constants, scalar helpers, and internal function declarations.

This layout is relevant because it stabilizes the order in which the preprocessor sees:

1. platform/compiler headers;
2. public API surface;
3. internal flags and dispatch macros;
4. SIMD degree and helper declarations.

That is the “posição estudada dos includes” in technical terms: include order is part of the compilation contract.

### 2.7 Optional TBB recursion path

`c/blake3.c` threads a `use_tbb` boolean through the internal update path. Public single-threaded update calls the base path with `false`; `blake3_hasher_update_tbb` calls it with `true` when `BLAKE3_USE_TBB` is compiled.

`blake3_compress_subtree_wide` remains the internal wide-subtree engine. Under `BLAKE3_USE_TBB`, it delegates the left/right recursive split to `blake3_compress_subtree_wide_join_tbb`.

`c/blake3_tbb.cpp` implements that join with `oneapi::tbb::parallel_invoke`, and enforces a no-exceptions contract with:

```cpp
static_assert(TBB_USE_EXCEPTIONS == 0,
              "This file should be compiled with C++ exceptions disabled.");
```

### 2.8 CI warning and matrix gates

The workflow sets:

```yaml
BLAKE3_CI: "1"
RUSTFLAGS: "-D warnings"
```

This makes warnings meaningful. The CI matrix covers Rust feature combinations, SIMD exclusions, C bindings, cross targets including ARMv7/AArch64, CMake with TBB on/off, pkg-config with TBB on/off, TinyCC portable build, MSRV build, Wasm, and TBB Rust-binding tests.

This is a real operational point: warnings and flags are not decorative; they are part of the gatekeeping model.

## 3. What should not be claimed without additional artifacts

The following should be marked `TOKEN_VAZIO` until benchmark/proof artifacts exist:

- “10–25% faster” as a universal result;
- “30–50% lower streaming latency” as a universal result;
- “speedup almost linear” without hardware, input-size, compiler, and thread-count tables;
- “CUDA support” as implemented, unless a CUDA source/build path is added and tested;
- “Makefile optimization path” unless a Makefile exists and is maintained;
- “Rust production binding” if the binding crate itself says it is intended for testing/benchmarking only;
- “superior to upstream” as a blanket statement.

Preferred wording:

> This fork exposes a more explicit build-orchestration surface around SIMD, warnings, C/C++ flags, optional TBB, and binding tests. Performance claims must be reported per target, compiler, feature set, and workload.

## 4. Defensible comparison model: not an arithmetic mean

Do not compare official BLAKE3 and this fork by a single average score. That creates the same failure as averaging a starving and overfed system.

Use axis-specific claims:

| Axis | Current defensible statement |
|---|---|
| Cryptographic primitive | Preserved; no documented change to compression constants or security model. |
| C public API | Extended with `BLAKE3_RESTRICT` and optional TBB update gate. |
| Build pipeline | More explicit through Cargo/CMake/CI flags and environment-gated rebuild behavior. |
| Warnings | Meaningful in CI through `BLAKE3_CI` and `RUSTFLAGS=-D warnings`. |
| SIMD dispatch | Maintains runtime x86 dispatch and compile-time NEON/Wasm paths. |
| TBB | Optional parallel recursion path; disabled if unavailable. |
| Android/Termux | Plausible target via CMake/clang/rust, but needs target-specific benchmark artifacts. |
| Documentation | Was partially promotional/inexact; now claim-gated. |

## 5. Recommended benchmark artifact before stronger claims

Create a reproducible benchmark file, for example `benchmarks/RAFAELIA_BUILD_MATRIX.md`, with:

- CPU model and core count;
- OS and kernel;
- compiler versions: `cc --version`, `clang --version`, `rustc -Vv`, `cmake --version`;
- flags: `CFLAGS`, `RUSTFLAGS`, CMake options;
- workload sizes: 1 KiB, 16 KiB, 128 KiB, 1 MiB, 64 MiB, 1 GiB;
- modes: portable, NEON, x86 asm, x86 intrinsics, TBB off, TBB on;
- output hash validation against official test vectors;
- throughput and wall-clock medians over repeated runs;
- energy/thermal notes only when measured.

## 6. RAFAELIA integration layer

Within RAFAELIA, BLAKE3 should be documented as an integrity primitive used by a wider custody system:

```text
input corpus
→ normalized chunk
→ BLAKE3/SHA3 digest
→ hashchain
→ Bitraf64 / ZIPRAF metadata
→ RAFCODE-Φ signature layer
→ GitHub/Termux reproducibility record
```

This is not a claim that BLAKE3 itself became RAFAELIA. It is the opposite: BLAKE3 stays precise, and RAFAELIA uses it as a measured vertebra in a larger verification body.

## 7. Operational conclusion

The strongest accurate claim is:

> This repository is an upstream-compatible BLAKE3 fork whose notable work is in build orchestration, compiler signaling, optional TBB recursion, warning discipline, and RAFAELIA-style integrity/custody documentation. Its value must be evaluated by axis, not by a single average.

Retroalimentação técnica: `F_ok = flags/warnings/pipeline reais`; `F_gap = benchmark matrix still missing`; `F_next = publish target-specific measurements before performance superiority claims`.
