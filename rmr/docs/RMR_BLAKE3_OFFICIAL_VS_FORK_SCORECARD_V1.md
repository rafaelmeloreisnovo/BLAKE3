<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR BLAKE3 — Official vs Fork Scorecard V1

State: ACTIVE_EVIDENCE_SCORECARD  
Claim allowed: false

## Baseline

- Official: `BLAKE3-team/BLAKE3@6aab490a26124663329dfd3961b8469f8fdb158b`
- Fork snapshot: `rafaelmeloreisnovo/BLAKE3@749629193bf796f0ce278a66eb5b76bbf884a15e`
- GitHub history relation at capture: **695 commits ahead / 28 behind**.

That proves a strongly divergent fork history. It does not by itself prove
better performance.

## Direct VS

| Surface | Official | Fork/RMR | Evidence state |
| --- | --- | --- | --- |
| C core throughput | reference | modified core | **PARITY / noise not separated** |
| Source hints | baseline | `BLAKE3_LIKELY`, `BLAKE3_RESTRICT` etc. | implemented + ablated |
| SIMD families | SSE/AVX/NEON/etc. | same families + RMR instrumentation | PASS |
| ARMv7 NEON | supported | historical device receipt | **+23.53% vs fork portable** |
| Public ABI | baseline | retained | **PASS** |
| Global symbols | baseline | 0 extra / 0 missing | **PASS** |
| GNU stack | baseline | non-executable | **PASS** |
| Static archive | 99,830 B | 99,990 B | +160 B observed |
| Rust/LTO | upstream profile | LTO + codegen-units=1 | exact-master V3 pending |
| b3sum | upstream CLI | buffering/implementation changes | exact-master V3 pending |
| TBB | optional path | optional path | exact-master V3 pending |
| Cross-arch | source portability | 7-target compile contract | **7/7 PASS** |
| HWIF/topology | not upstream scope | RMR extension | implemented |
| SIMPERF | not upstream scope | RMR extension | implemented |
| Crypto runtime | not upstream scope | 15 algorithms / 135 cells | implemented |
| Physical ARM current VS | — | device runner prepared | TOKEN_VAZIO |
| Independent reproduction | — | — | TOKEN_VAZIO |

## What is already established

The fork is not merely upstream plus documentation. It contains source, build,
CLI and architecture changes plus a substantial RMR execution/audit layer.

The observed C-core comparison on x86-64 does **not** establish a broad
throughput superiority. Multi-size paired runs classify the core as parity or
noise-not-separated.

The historical ARMv7 receipt establishes a different point: inside the fork,
NEON materially outperformed the portable backend on the recorded device:

```text
portable = 132.313 MiB/s
NEON     = 163.450 MiB/s
delta    = +23.53%
```

That is an ISA/backend result, not an official-vs-fork result.

## What still decides whether the fork is faster

The strongest remaining surfaces are:

1. Rust release/LTO;
2. b3sum hashing/checkfile/mmap behavior;
3. oneTBB parallel-tree path;
4. current ARMv7 official-vs-fork physical run;
5. AArch64 official-vs-fork physical run;
6. end-to-end RMR pipeline.

The canonical V3 executors already exist for the first three. Device-bound
execution remains required for ARM.

## Bottom line

```text
FORK != OFFICIAL
FORK_HISTORY = STRONGLY_DIVERGED
C_CORE_X86 = PARITY_ON_OBSERVED_RUNNERS
SYSTEM_SCOPE_FORK > UPSTREAM_SCOPE
UNIVERSAL_SPEED_SUPERIORITY = NOT_ESTABLISHED
```

Any later improvement claim must name the exact surface, architecture, workload
and receipt.
