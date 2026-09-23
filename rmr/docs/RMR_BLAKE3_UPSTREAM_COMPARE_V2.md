<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR BLAKE3 upstream comparison V2

State: IMPLEMENTED_PENDING_EXACT_HEAD_V2  
Claim allowed: false

## Goal

Compare the fork C core against current official BLAKE3 with the same host,
compiler, benchmark source and work volume.

Current upstream master/pin:

`6aab490a26124663329dfd3961b8469f8fdb158b`

## Existing evidence

The latest prior common-harness receipt observed:

```text
official median = 6654.876 MiB/s
fork median     = 6698.333 MiB/s
median delta    = +0.653%
ratio           = 1.006530x
KAT             = PASS
digest equality = PASS
```

Raw rounds:

```text
official: 6764.638, 6645.915, 6654.876
fork:     6761.808, 6627.688, 6698.333
```

CV is about 1% on both sides, while the mean delta is about +0.112%.
Therefore the previous receipt establishes parity with a small observed median
advantage, not yet a statistically separated speedup.

## Fork changes with plausible performance relevance

- branch prediction hints in CPU dispatch/AVX-512 hot path;
- restrict annotations in the C API;
- Rust release/bench LTO and one codegen unit;
- b3sum fixed-array key input and 64 KiB checkfile buffering.

The AArch64 no-op ABI marker is not a hash acceleration by itself.

## V2 method

`rmr/tools/orchestrate_blake3_compare_v2.sh` compares official
`c/libblake3` and fork `c/libblake3` using the same
`blake3_size_bench.c`.

Default sizes: 64 B, 256 B, 1 KiB, 4 KiB, 64 KiB, 1 MiB, 16 MiB.

Each size uses warmup, 9 paired rounds, alternating order and the same target
work volume. KAT and digest equality are mandatory. The analyzer emits
per-size median/mean/CV plus paired bootstrap 95% intervals.

Per-size states:

- OBSERVED_FASTER_ON_THIS_RUNNER
- OBSERVED_SLOWER_ON_THIS_RUNNER
- PARITY_OR_NOISE_NOT_SEPARATED

## Boundary

```text
small-message gain != stream gain
x86-64 gain != ARM gain
core gain != b3sum gain
RMR pipeline gain != primitive gain
CI observation != universal superiority
```

Physical ARMv7/AArch64 receipts remain required for cross-architecture claims.
