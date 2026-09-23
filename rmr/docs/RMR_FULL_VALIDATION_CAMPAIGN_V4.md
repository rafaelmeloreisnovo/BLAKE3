<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Full Validation Campaign V4

State: EXECUTION_REQUIRED  
Claim allowed: false

## Goal

Validate the fork against official BLAKE3 across correctness, performance,
causal source changes, backends, build/link/binary surfaces, Rust/CLI,
cross-architecture compilation and the surrounding RMR runtime.

The campaign is deliberately wider than a single throughput benchmark.

## Baseline

Official reference:

```text
BLAKE3-team/BLAKE3
6aab490a26124663329dfd3961b8469f8fdb158b
```

Fork evidence must always record its exact head separately.

## Axes

1. **Source provenance** — enumerate changed files and identify changes with
   plausible performance relevance.
2. **C correctness/quality** — strict warnings, CTest, semantic modes,
   alignment sweep, ASan/UBSan and installed-library consumer.
3. **C core performance** — common harness, multiple sizes, paired/alternating
   runs and statistical classification.
4. **C source ablation** — fork, no-`likely`, no-`restrict`, no-hints.
5. **SIMD/backend matrix** — portable, SSE2, SSE4.1, AVX2, automatic ASM and
   automatic intrinsics, with digest equivalence.
6. **oneTBB** — isolate parallel-tree behavior from single-thread SIMD.
7. **ABI/ELF** — public ABI, global symbols, binary/archive size, sections,
   relocations and non-executable GNU stack.
8. **Rust library/profile** — official versus fork plus release-profile/LTO
   ablation.
9. **b3sum CLI** — normal hashing, mmap/no-mmap, single-thread behavior and
   large checkfile parsing/buffering.
10. **Cross-architecture contract** — x86-64, x86-32, ARMv7, AArch64, WASM32,
    RISC-V64 and PPC64LE.
11. **RMR integration** — topology, HWIF, crypto runtime, SIMPERF, fixed256,
    freestanding custody and PAI42.
12. **Physical ARM** — never inferred from cross compilation.
13. **Physical IOPS** — never inferred from logical I/O or CI storage.
14. **Independent reproduction** — remains TOKEN_VAZIO until an external
    receipt exists.

## Promotion rules

```text
compile != execute
CI runner != physical device
median delta != statistical separation
x86 result != ARM result
fork core != b3sum != RMR pipeline
simulated performance != measured performance
```

A performance cell may be promoted only when the receipt identifies exact
source commits, compiler, flags, backend, workload, input size, repetitions,
timing method and uncertainty.

## Expected documentation

The aggregated receipt must preserve both positive and negative results.
A FAIL or parity result is evidence and must not be overwritten by a later
PASS. The final report should state where the fork is equivalent to upstream,
where a fork change has a causal effect, where a regression exists, and where
the evidence is still TOKEN_VAZIO.

## Canonical executors

The execution surface is declared in
`rmr/validation/FULL_VALIDATION_MANIFEST_V4.json`.

The main CI entrypoints are:

- `.github/workflows/rmr-upstream-comprehensive-v3.yml`;
- `.github/workflows/rmr-full-validation.yml`.

V3 provides the deepest official-vs-fork decomposition. The full-validation
workflow independently aggregates the broad RMR evidence surface.

## Claim boundary

No repository-wide "faster than upstream" claim is emitted automatically.
Any supported improvement must name the exact surface and scope, for example:

```text
B3SUM_CHECKFILE_BUFFER @ x86_64 runner
C_DISPATCH_LIKELY @ specific sizes
ARMV7_NEON @ specific physical device
RUST_LTO @ exact build profile
```

Everything else remains parity, FAIL, REVIEW or TOKEN_VAZIO as observed.


## Exact-master gate

The two canonical campaign workflows run on both pull requests and pushes to
`master`:

- `.github/workflows/rmr-full-validation.yml`;
- `.github/workflows/rmr-upstream-comprehensive-v3.yml`.

This prevents a fast merge from leaving the comprehensive validation in a
pre-job `pending` state. The post-merge push becomes the canonical exact-master
execution receipt.

```text
PR_HEAD validation != MASTER_PUSH validation
MERGE != VALIDATION
MASTER_PUSH_SUCCESS = exact-master CI evidence
```


## Physical telemetry gate

The device-bound runner records privacy-safe runtime context before and after
the benchmark:

- current/min/max CPU frequency where the kernel exposes cpufreq;
- scaling governor;
- thermal-zone type and raw temperature value;
- MemTotal/MemFree/MemAvailable;
- logical CPU count.

The analyzer
`rmr/validation/analyze_physical_telemetry.py` materializes
`telemetry-delta.json`.

A physical speed result may be reported without available telemetry, but its
interpretation must explicitly preserve:

```text
THERMAL/DVFS = TOKEN_VAZIO_PLATFORM_TELEMETRY_UNAVAILABLE
```

when the platform does not expose these surfaces.

When telemetry exists:

```text
THROUGHPUT_DELTA != THROTTLING_CAUSALITY
FREQUENCY_DELTA != PERFORMANCE_CAUSALITY
THERMAL_DELTA != REGRESSION_CAUSALITY
```

The measurements are contextual evidence, not automatic causal attribution.
