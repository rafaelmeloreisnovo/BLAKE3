# BLAKE3 fork — RAFAELIA build orchestration notes

This repository is a fork/distribution of the BLAKE3 implementation. It is not affiliated with the official BLAKE3 team.

The correct technical framing is:

> BLAKE3 remains the cryptographic primitive. This fork documents and extends the build, warning, flag, dispatch, optional TBB, dimensional-layout, and RAFAELIA integrity/custody layers around it.

This repository should not be described by a single average score. A single score hides the difference between cryptographic semantics, compiler behavior, build reproducibility, runtime dispatch, optional parallel recursion, binary-size geometry, and RAFAELIA custody metadata.

## Upstream BLAKE3 summary

BLAKE3 is a cryptographic hash function that is:

- fast compared with older common hashes;
- secure against known weaknesses that break MD5/SHA-1;
- highly parallelizable because it is internally a Merkle tree;
- suitable for regular hashing, keyed hashing, key derivation, MAC-like use, and extendable output;
- implemented in Rust and C with SIMD-oriented backends.

For the upstream specification and design rationale, see the BLAKE3 paper and the official BLAKE3 repository.

## Fork boundary

This fork keeps a strict boundary:

| Layer | Position |
|---|---|
| Cryptographic primitive | Preserve compatibility and validate with test vectors. |
| C/Rust build pipeline | Make flags, warnings, target selection, and rebuild triggers explicit. |
| Dimensional execution geometry | Document 32/64/1024-byte units, CV stack size, SIMD degree, lazy merge, and ABI-sensitive state layout. |
| SIMD dispatch | Keep target-specific routing observable and testable. |
| Optional TBB | Add a gated parallel subtree path without making TBB mandatory. |
| RAFAELIA/Bitraf | Use BLAKE3 as an integrity primitive inside a wider custody/hashchain system. |
| RMR | Snapshot/cadeia forense de operação: binário, flags, host, target, logs, digest e reprodutibilidade. |
| FiberHaga | Separate RMR/RAFAELIA hashing core; not a BLAKE3 layer and not included as a BLAKE3 claim. |
| Claims | Use benchmark and CI evidence; avoid blanket superiority language. |

## What is technically present

### Rust build profile

`Cargo.toml` configures release and bench profiles with LTO enabled and one codegen unit. This favors whole-program optimization and stable code generation for hash workloads.

### Build-script orchestration

The root `build.rs` acts as a precompiler-level router:

- observes relevant environment variables such as `CC` and `CFLAGS`;
- treats warnings as errors when `BLAKE3_CI` is set;
- avoids broad rebuild noise from unrelated environment changes;
- chooses assembly, intrinsics, NEON, Wasm SIMD, or pure Rust paths according to target and features;
- checks AVX-512 compiler support before enabling the AVX-512 C path;
- emits known cfg names to keep newer Rust warning behavior controlled.

### Dimensional execution geometry

The implementation is shaped around binary-friendly units:

```text
OUT_LEN = 32 bytes
BLOCK_LEN = 64 bytes
CHUNK_LEN = 1024 bytes = 16 blocks
MAX_DEPTH = 54
cv_stack = (MAX_DEPTH + 1) * OUT_LEN = 1760 bytes
```

The dimensional audit documents how these values affect stack pressure, subtree splitting, lazy merge behavior, SIMD degree, ABI-sensitive padding, and ARM/Termux measurement requirements.

### CMake/C pipeline

The C build exposes controlled options:

```bash
cmake -S c -B c/build -DCMAKE_BUILD_TYPE=Release
cmake --build c/build
```

Optional oneTBB:

```bash
cmake -S c -B c/build -DCMAKE_BUILD_TYPE=Release -DBLAKE3_USE_TBB=ON
cmake --build c/build
```

Optional oneTBB fetch:

```bash
cmake -S c -B c/build -DCMAKE_BUILD_TYPE=Release \
  -DBLAKE3_USE_TBB=ON \
  -DBLAKE3_FETCH_TBB=ON
cmake --build c/build
```

### C header and include discipline

The public C header currently includes `<stddef.h>` and `<stdint.h>`, defines visibility macros, and uses `BLAKE3_RESTRICT` on selected public API pointer parameters for GCC/Clang.

Documentation must not claim those standard headers were removed from `blake3.h`; the current code keeps them.

### Optional TBB path

When compiled with `BLAKE3_USE_TBB`, the C API exposes `blake3_hasher_update_tbb`. Internally, the update path carries a `use_tbb` boolean and the wide-subtree recursion can use `oneapi::tbb::parallel_invoke`.

### CI warning discipline

The GitHub workflow sets:

```yaml
BLAKE3_CI: "1"
RUSTFLAGS: "-D warnings"
```

The workflow also tests Rust features, SIMD exclusion modes, C bindings, cross targets, CMake builds, pkg-config builds, TinyCC portable builds, Wasm, and TBB bindings.

## RAFAELIA/Bitraf custody layer

Within RAFAELIA, BLAKE3 is best documented as one measurable vertebra in a larger chain:

```text
input/corpus
→ normalized chunk
→ BLAKE3/SHA3 digest
→ hashchain
→ Bitraf64 / ZIPRAF metadata
→ RAFCODE-Φ signature layer
→ GitHub/Termux reproducibility record
```

This keeps the claim clean: BLAKE3 is the hash primitive; RAFAELIA supplies orchestration, custody, metadata, symbolic indexing, and reproducibility context.

## RMR / BLAKE3 / FiberHaga boundary

RMR is not BLAKE3. BLAKE3 is a digest primitive inside an RMR custody chain. FiberHaga is not BLAKE3 and is not a “perfumaria” layer on top of BLAKE3; it is a separate RMR/RAFAELIA core to be specified and benchmarked separately.

Use this framing:

```text
BLAKE3 = public hash primitive and compatibility baseline
BLAKE3 fork = binary/build/flags/dispatch/custody orchestration without changing the core
RMR = machine-operation snapshot and forensic custody chain
FiberHaga = separate RMR/RAFAELIA hashing core
RAFAELIA = orchestration, custody, metadata, reproducibility and claim gates
```

## Claim gates

Do not use broad language such as “globally superior to upstream” without target-specific evidence.

Use these gates instead:

| Claim | Status |
|---|---|
| Cryptographic compatibility | Defensible when test vectors pass. |
| Build orchestration is explicit | Defensible from `build.rs`, CMake, and CI. |
| Warning discipline exists | Defensible from `BLAKE3_CI` and `RUSTFLAGS=-D warnings`. |
| Optional TBB path exists | Defensible when built with `BLAKE3_USE_TBB`. |
| Dimensional geometry exists | Defensible from constants, structs, SIMD degree, CV stack, and subtree logic. |
| Exact ABI sizes on Android/Termux | Measure with `tools/sizeof_blake3_state.c` before claiming. |
| CUDA support exists | Do not claim unless implemented and tested. |
| Fixed percentage speedups | Do not claim without benchmark matrix. |
| 10–20x same-engine speedup | Treat as local/preliminary until artifacts are attached. |
| RMR custody chain | Defensible as architecture when manifest/log/digest are present. |
| FiberHaga performance | Not a BLAKE3 claim; requires separate spec and benchmark. |
| Single average score comparison | Avoid; it hides extremes and target-specific behavior. |

## Documentation map

- [`c/README.md`](c/README.md): C API, build commands, TBB/NEON notes, and C-specific claim gates.
- [`docs/RAFAELIA_BLAKE3_BUILD_ORCHESTRATION.md`](docs/RAFAELIA_BLAKE3_BUILD_ORCHESTRATION.md): deeper audit of flags, warnings, include order, build routing, CI, and RAFAELIA custody interpretation.
- [`docs/rafaelia/RMR_INFRAESTRUTURA_CRITICA_DADOS_IA_CUSTODIA_BLAKE3.md`](docs/rafaelia/RMR_INFRAESTRUTURA_CRITICA_DADOS_IA_CUSTODIA_BLAKE3.md): RMR custody, infrastructure-critical data, algorithmic audit, logs, and proof-of-conformity framing.
- [`docs/rafaelia/RMR_LICENSE_FIBER_H_BOUNDARY.md`](docs/rafaelia/RMR_LICENSE_FIBER_H_BOUNDARY.md): license boundary, BLAKE3/RAFAELIA separation, and FiberHaga claim gates.
- [`docs/rafaelia/RMR_BLAKE3_BINARY_ORCHESTRATION_VS_FIBERHAGA.md`](docs/rafaelia/RMR_BLAKE3_BINARY_ORCHESTRATION_VS_FIBERHAGA.md): explicit split between same-engine BLAKE3 binary orchestration, RMR forensic custody, and FiberHaga as separate core.

## Termux / Android note

For Termux-style local builds, start with:

```bash
pkg update
pkg install git clang cmake rust
cmake -S c -B build-termux -DCMAKE_BUILD_TYPE=Release
cmake --build build-termux
```

For ARMv7/Android targets, do not assume desktop flags. Document the actual target triple, compiler, NEON support, ABI, and measured results.

To measure target layout:

```bash
cc -std=c11 -Ic tools/sizeof_blake3_state.c -o sizeof_blake3_state
./sizeof_blake3_state
```

## Retrofeedback

`F_ok`: BLAKE3 is preserved as primitive; the fork's real work is build orchestration and custody integration.  
`F_gap`: target-specific benchmark matrix is still needed for strong performance claims, including any 10–20x estimate or FiberHaga comparison.  
`F_next`: add reproducible benchmark artifacts before claiming percentage speedups, production superiority, or FiberHaga baseline advantage.

[@oconnor663]: https://github.com/oconnor663
[@sneves]: https://github.com/sneves
[@veorq]: https://github.com/veorq
[@zookozcash]: https://github.com/zookozcash
