<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR BLAKE3 Full Validation Matrix V1

State: IMPLEMENTED_PENDING_EXACT_HEAD_EXECUTION  
Claim allowed: false

## Objective

Validate all materially distinct differences between official BLAKE3, the
modified fork and the wider RMR system without collapsing them into a single
"faster/slower" statement.

```text
UPSTREAM_BLAKE3
!=
FORK_BLAKE3_CORE
!=
FORK_RUST_BUILD
!=
B3SUM_CLI
!=
RMR_FULL_SYSTEM
```

## 1. Correctness and safety

The C implementation runs official test vectors in hash, XOF, keyed-hash and
derive-key modes under ASan/UBSan, for intrinsics and assembly builds.

Rust library and b3sum test suites execute independently.

Digest equality is required before any performance comparison is accepted.

## 2. Current upstream vs fork core

The V2 common harness compares current official upstream against the exact fork
head with identical compiler, CMake release profile, TBB state, input generator,
warmup, work volume and alternating paired rounds.

Sizes:

- 64 B
- 256 B
- 1 KiB
- 4 KiB
- 64 KiB
- 1 MiB
- 16 MiB

Per-size output includes median, mean, coefficient of variation and paired
bootstrap interval.

## 3. C source ablation

The fork is rebuilt as:

- intact fork;
- fork without BLAKE3_LIKELY;
- fork without BLAKE3_RESTRICT;
- fork without both hints;
- current upstream.

The same digest/performance harness is used. This isolates whether source hints
actually contribute rather than crediting the entire fork.

## 4. SIMD backend matrix

The fork is measured as:

- portable;
- SSE2 maximum;
- SSE4.1 maximum;
- AVX2 maximum;
- native dispatch.

Every backend must produce the same digest. The runtime SIMD degree is recorded.

AVX-512 remains represented by native dispatch when the runner exposes it; a
named AVX-512 performance claim requires the runtime/backend evidence to show
that path was actually selected.

## 5. Rust build profile

The same Rust benchmark source is compiled against:

- official upstream;
- fork tuned with its release LTO/codegen-units profile;
- fork with LTO disabled and codegen-units=16.

This separates Rust profile effects from C-core and CLI effects.

## 6. b3sum

Official and fork b3sum binaries run:

- 1 MiB hash;
- 64 MiB hash;
- 64 MiB single-thread/no-mmap hash;
- 20,000-line checkfile over an empty file.

The last case targets checkfile parsing/buffering without allowing large hash
work to dominate the measurement.

## 7. Binary/linker/symbol surface

Official and fork artifacts are compared for:

- public BLAKE3 API symbols;
- fork-only/missing global symbols;
- archive and executable size;
- ELF headers and sections;
- GNU_STACK executability;
- strict-warning build state;
- SHA-256 artifact receipts.

## 8. Cross-architecture contracts

The public C API contract is compiled for:

- x86-64;
- x86-32;
- ARMv7;
- AArch64;
- WASM32;
- RISC-V64;
- PPC64LE.

Existing HWIF and crypto contract matrices run in the same campaign.

A compile success is not a physical-execution success.

## 9. Physical evidence

ARMv7, AArch64 and physical IOPS are promoted only by device-bound receipts.
CI simulation/cross-compilation does not substitute for device evidence.

## 10. Full receipt

The workflow aggregates all available artifacts into:

- RMR_FULL_VALIDATION_RECEIPT_V1.json
- RMR_FULL_VALIDATION_RECEIPT_V1.md

Each axis remains independently typed PASS, FAIL, REVIEW or TOKEN_VAZIO.

## Prior evidence

Before this full campaign, exact-head V2 x86-64 testing found statistical
parity between current upstream and fork C core across seven sizes. Historical
ARMv7 evidence separately found the fork NEON path approximately 23.53% faster
than the fork portable path on that device/configuration.

These observations remain scoped to their receipts and do not become universal
architecture claims.

## Promotion rule

```text
SOURCE
-> BUILD
-> KAT
-> BACKEND_IDENTITY
-> PERFORMANCE_MEASUREMENT
-> STATISTICAL_ANALYSIS
-> DEVICE/WORKLOAD_SCOPE
-> CLAIM
```

Skipping a gate is not allowed.
