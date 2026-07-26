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

## Fork versus official upstream

Run from the fork root:

```bash
chmod +x c/tools/benchmark_fork_vs_upstream_termux_armv7.sh
c/tools/benchmark_fork_vs_upstream_termux_armv7.sh
```

The runner:

1. checks out the official `BLAKE3-team/BLAKE3` repository;
2. captures both commit hashes;
3. builds both implementations with the same Clang, CMake generator, `-O3`, ARMv7, NEON and softfp flags;
4. verifies the `abc` test vector in both builds;
5. links the same in-memory benchmark source against each static library;
6. alternates execution order across rounds;
7. checks digest equivalence;
8. records medians, ratio, raw CSV, receipt and SHA-256 hashes.

Environment overrides:

```bash
ROUNDS=7 ITERATIONS_MIB=1024 JOBS=2 \
  c/tools/benchmark_fork_vs_upstream_termux_armv7.sh
```

Claim state before that runner completes:

```text
fork_NEON_vs_fork_portable=PASS
fork_vs_official_upstream=TOKEN_VAZIO
```
