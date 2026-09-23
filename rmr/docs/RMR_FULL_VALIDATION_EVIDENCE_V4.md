<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Full Validation Evidence V4

State: EVIDENCE_LEDGER_ACTIVE  
Claim allowed: false

This document preserves observed results by exact run/SHA while the current
exact-master comprehensive campaign is still queued.

## Current reference

- official: `BLAKE3-team/BLAKE3@6aab490a26124663329dfd3961b8469f8fdb158b`
- fork master: `fc73b1ddb3f791f892bb25eb3f0acf82b18a30af`
- GitHub history relation at capture: 665 commits ahead / 28 behind.

The history relation proves divergence only. It is not a performance metric.

## C core: official vs fork

Prior full-validation run `35836637289` measured the common C harness:

| Size | Official MiB/s | Fork MiB/s | Delta | State |
| ---: | ---: | ---: | ---: | --- |
| 64 B | 697.306 | 696.653 | -0.094% | parity/noise |
| 256 B | 844.626 | 845.832 | +0.143% | parity/noise |
| 1 KiB | 839.971 | 841.381 | +0.168% | parity/noise |
| 4 KiB | 2601.513 | 2603.929 | +0.093% | parity/noise |
| 64 KiB | 4057.554 | 4048.965 | -0.212% | parity/noise |
| 1 MiB | 4023.306 | 4016.055 | -0.180% | parity/noise |
| 16 MiB | 3703.768 | 3659.109 | -1.206% | parity/noise |

All rows retained digest equivalence. No universal C-core speedup is promoted.

## C source ablation

The prior ablation compared:

```text
fork
fork-no-likely
fork-no-restrict
fork-no-hints
upstream
```

Observed hint/restrict deltas were small. The largest highlighted removal was
`no-likely @ 1 MiB = -0.565%`; other selected deltas were
`no-restrict @ 1 KiB = +0.329%` and
`no-hints @ 64 KiB = +0.278%`.

Interpretation:

```text
LIKELY/RESTRICT_EXIST = true
BROAD_CAUSAL_SPEEDUP_FROM_HINTS = NOT_ESTABLISHED
```

## SIMD/backend surface

Prior same-fork backend receipts:

| Backend | SIMD degree | 64 B MiB/s | 1 MiB MiB/s |
| --- | ---: | ---: | ---: |
| portable | 1 | 567.509 | 661.267 |
| SSE2 | 4 | 613.403 | 1757.389 |
| SSE4.1 | 4 | 697.220 | 2035.346 |
| AVX2 | 8 | 696.510 | 4012.512 |
| native | 8 | 694.411 | 4008.846 |

At 1 MiB, AVX2 was about 6.07x the portable path on that runner. This is a
backend/ISA result, not a fork-vs-upstream claim.

## Binary / ABI / linker

Observed prior run:

- public API contract: PASS;
- strict warnings: PASS;
- GNU stack: non-executable;
- fork extra global symbols: 0;
- upstream symbols missing from fork: 0;
- benchmark executable: 97,768 bytes on both sides;
- static archive: upstream 99,830 bytes; fork 99,990 bytes; delta +160 bytes.

## Cross architecture

Provider-neutral/public C contract compile status:

```text
x86_64  PASS
x86_32  PASS
ARMv7   PASS
AArch64 PASS
WASM32  PASS
RISC-V64 PASS
PPC64LE PASS
```

Compilation is not physical execution.

## Physical evidence

Historical ARMv7 device receipt inside the fork:

```text
portable = 132.313 MiB/s
NEON     = 163.450 MiB/s
delta    = +23.53%
```

This remains valid only for its recorded device/version. Current strict
official-vs-fork ARMv7 and AArch64 receipts remain
`TOKEN_VAZIO_PHYSICAL`.

The canonical current device runner is
`rmr/tools/run_physical_upstream_compare_v3.sh`.

## Current exact-master campaign

For fork master `fc73b1ddb3f791f892bb25eb3f0acf82b18a30af`:

- Comprehensive V3 run `35839291725`: QUEUED;
- Full Validation run `35839291770`: QUEUED.

Historical receipts are not silently promoted to exact-master results.

## Boundaries

```text
HISTORICAL_RECEIPT != CURRENT_EXACT_MASTER
DIVERGED_HISTORY != PERFORMANCE_SUPERIORITY
SIMD_BACKEND_GAIN != FORK_GAIN
C_CORE_PARITY != CLI_PARITY
CROSS_COMPILE_PASS != PHYSICAL_EXECUTION
SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM
```
