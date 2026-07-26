# ARMv7 benchmark evidence

This directory stores reproducible, scope-limited performance evidence for the C implementation on Android/Termux ARMv7.

## Recorded device result — 2026-07-26

Commit `31419ed18928a9a25adc892c5eba9eef584d59f4` was built with Clang 21.1.8 and CMake 4.4.0 using `-O3 -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=softfp`.

Observed medians for a 1 MiB in-memory buffer repeated 512 times across five alternating rounds:

| Backend | Median throughput |
|---|---:|
| portable | 132.313 MiB/s |
| NEON | 163.450 MiB/s |

Observed ratio: `1.235x`.
Observed median gain: `23.53%`.
Cryptographic equivalence: `PASS`.

This supports only the scoped claim that the NEON build was faster than the portable build on the measured ARMv7 Termux device. It does not establish a universal speedup or superiority over the official upstream repository.

## Strict build contract for fork versus official upstream

The comparison runner does not suppress compiler warnings and does not silently relax the fork build. Both sides are compiled under the same strict contract.

### Mandatory diagnostic gate

```text
-Wall
-Wextra
-Wpedantic
-Werror
CMAKE_COMPILE_WARNING_AS_ERROR=ON
```

Warnings are diagnostics; they do not directly remove symbols. Their role here is to stop warning-bearing code from entering the measured binary.

### Code-generation and architecture contract

```text
-O3
-DNDEBUG
-march=armv7-a
-mfpu=neon-vfpv4
-mfloat-abi=softfp
-fvisibility=hidden
-ffunction-sections
-fdata-sections
-fno-asynchronous-unwind-tables
-fno-unwind-tables
```

### Linker and symbol-surface contract

```text
-Wl,--gc-sections
-Wl,--build-id=none
-Wl,--exclude-libs,ALL
```

These linker and visibility options are the part of the contract that removes unreachable sections, avoids build-id metadata and prevents static archive members from being promoted into the dynamic export surface.

The runner also records, for both builds:

- verbose configure and compilation logs;
- exact `compile_commands.json`;
- defined global symbols in each static archive;
- dynamic symbols and unresolved symbols in each executable;
- ELF section tables;
- section sizes;
- full static-library disassembly;
- SHA-256 for the evidence and binaries.

### Source-integrity boundary

The runner does **not** edit `blake3.c`, `blake3_dispatch.c`, `blake3_portable.c`, `blake3_neon.c` or the upstream cryptographic core. It therefore does not restore removed symbols, branches or algorithmic loops.

The loops in `c/bench_rmr.c` are deliberately outside the BLAKE3 library. They initialize a deterministic 1 MiB input, perform warm-up and repeat the same public hash call to accumulate enough elapsed time for measurement. The identical harness is linked against both libraries, so its cost is symmetric and its source is preserved in the evidence chain.

## Run fork versus official upstream

Run from the fork root:

```bash
chmod +x c/tools/benchmark_fork_vs_upstream_termux_armv7.sh
c/tools/benchmark_fork_vs_upstream_termux_armv7.sh
```

The runner:

1. checks out the official `BLAKE3-team/BLAKE3` repository;
2. captures both commit hashes;
3. builds both implementations with the same strict warning, code-generation, ARMv7, NEON and linker contract;
4. verifies the `abc` test vector in both builds;
5. links the same in-memory benchmark source against each static library;
6. alternates execution order across rounds;
7. checks digest equivalence;
8. records medians, ratio, raw CSV, logs, symbol tables, disassembly, receipt and SHA-256 hashes.

Environment overrides:

```bash
ROUNDS=7 ITERATIONS_MIB=1024 JOBS=2 \
  c/tools/benchmark_fork_vs_upstream_termux_armv7.sh
```

Claim state before that runner completes:

```text
fork_NEON_vs_fork_portable=PASS
fork_vs_official_upstream=TOKEN_VAZIO
strict_fork_vs_official_warning_gate=TOKEN_VAZIO
symbol_surface_comparison=TOKEN_VAZIO
```
