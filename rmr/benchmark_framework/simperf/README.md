<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR SIMPERF V1 — calibrated analytical performance emulator

## What it is

SIMPERF predicts performance profiles by calibrating against **measured official
code** on one reference profile and transferring the observed shape to other
hardware archetypes.

It is intentionally not QEMU and not a cycle-accurate ISA simulator.

```text
official measurement -> size curve -> profile scaling -> memory ceiling
                     -> optional observed RMR/official ratio
                     -> transferred estimate + uncertainty interval
```

Outputs include:

- ns/op;
- ops/s;
- MiB/s;
- low/high uncertainty interval;
- optional projected RMR/official speedup;
- exact input size.

## Why not use instruction emulation as performance truth?

Functional emulators are useful for correctness and ISA coverage, but their
host/JIT/device model changes timing. SIMPERF therefore uses physical receipts
as anchors and labels every cross-profile result `ANALOGY_ESTIMATE`.

## Five hardware archetypes

1. ARMv7 NEON mobile;
2. AArch64 NEON efficiency;
3. AArch64 NEON performance;
4. x86-64 AVX2 desktop;
5. x86-64 AVX-512 server.

These are deliberately generic profiles, not claims about a named commercial
CPU. Every numeric prior must be replaced or tightened by device receipts.

## Calibration CSV

```csv
algorithm,size_bytes,ns_per_op
blake3,64,55.0
blake3,1024,210.0
blake3,1048576,180000.0
```

Candidate/RMR measurements can use the same schema. If supplied, SIMPERF
computes the observed reference speedup and transfers only a configurable
fraction (default 70%) to avoid pretending perfect portability.

## Example

```sh
python3 rmr/benchmark_framework/simperf/simperf_model.py \
  --profiles rmr/benchmark_framework/simperf/hardware_profiles.json \
  --official official.csv \
  --candidate rmr.csv \
  --reference-profile x86_64-avx2-desktop \
  --target-profile armv7-neon-mobile \
  --target-profile aarch64-neon-performance \
  --out simperf.json
```

## Promotion gate

A simulated row never becomes a physical performance claim. Promotion requires
a matching device receipt and an error report:

```text
relative_error = abs(predicted - measured) / measured
```

The model should retain prediction, measurement and error append-only so failed
analogies remain evidence rather than being silently recalibrated away.
