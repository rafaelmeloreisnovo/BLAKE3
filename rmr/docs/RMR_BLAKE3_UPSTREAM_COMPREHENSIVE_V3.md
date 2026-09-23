<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR BLAKE3 upstream comprehensive validation V3

State: IMPLEMENTED / CI_EVIDENCE_PENDING  
Claim allowed: false

## Comparison identity

Official reference:

`BLAKE3-team/BLAKE3@6aab490a26124663329dfd3961b8469f8fdb158b`

Fork reference is always the exact pull-request head checked out by Actions.

The suite tests the fork as three distinct objects:

```text
BLAKE3 official primitive
!= BLAKE3 fork primitive
!= complete RMR system
```

## Gates

### 1. Source/provenance

Walk both trees, hash every file and classify changed paths into core C, Rust,
CLI, build/CI, RMR, audits, docs and other.

A changed source file is evidence of divergence, not evidence of improvement.

### 2. C correctness

Both implementations build with strict warnings. The same differential probes
exercise:

- ordinary hash;
- keyed hash;
- derive-key;
- XOF;
- finalize_seek;
- reset;
- incremental updates with several chunk sizes;
- input offsets 0..63 and boundary sizes around block/chunk lengths.

Outputs must be byte-identical.

### 3. Sanitizers

Portable builds run under AddressSanitizer + UndefinedBehaviorSanitizer.
This gate is correctness/safety evidence, not performance evidence.

### 4. CMake/install/pkg-config

Both implementations are installed into isolated prefixes, then a separate
consumer is compiled only through installed pkg-config metadata and must
reproduce the BLAKE3 abc KAT.

### 5. Core performance

The V2 common-harness comparator is retained:

- 64 B;
- 256 B;
- 1 KiB;
- 4 KiB;
- 64 KiB;
- 1 MiB;
- 16 MiB;
- 9 paired rounds;
- alternating order;
- identical target work;
- bootstrap interval.

### 6. x86 backend/dispatch

Both sides are built as:

- portable;
- SSE2 ceiling;
- SSE4.1 ceiling;
- AVX2 ceiling;
- automatic AMD64 assembly;
- automatic x86 intrinsics.

Each records actual SIMD degree, KAT, digest and throughput.

### 7. oneTBB

The C parallel update path is tested independently at 1 MiB and 16 MiB.
TBB performance is not attributed to the scalar/SIMD primitive.

### 8. ABI/symbol/ELF

The suite records:

- public constants;
- sizeof public structures;
- public-field offsets;
- defined/undefined global symbols;
- ELF sections/program headers;
- executable and archive sizes;
- GNU_STACK execution state.

### 9. Rust and b3sum

Runs Rust library/CLI tests and builds:

- official release;
- fork release;
- fork release with LTO disabled and codegen-units=16.

It measures:

- 64 MiB hash with one thread;
- 64 MiB hash with four threads;
- 20k-line checkfile workload;
- binary size;
- digest equivalence.

This separates source changes from the fork's release-profile changes.

### 10. Cross architecture

C compilation:

- x86-64;
- x86-32;
- ARMv7 portable + NEON;
- AArch64 portable + NEON;
- RISC-V64 portable;
- PPC64LE portable.

Rust compilation:

- wasm32-unknown-unknown.

Cross compilation is not physical execution.

### 11. RMR integration

The fork-only integration gate includes topology, HWIF, crypto runtime,
SIMPERF, CTest, fixed256 probes, freestanding custody and PAI42 where the
corresponding runner is present.

## Interpretation law

```text
SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM

CORE_GAIN != CLI_GAIN
CLI_GAIN != BUILD_GAIN
X86_GAIN != ARM_GAIN
SIMD_GAIN != TBB_GAIN
RMR_PIPELINE_GAIN != BLAKE3_PRIMITIVE_GAIN
COMPILE_PASS != PHYSICAL_EXECUTION
MEDIAN_DELTA != STATISTICAL_SEPARATION
```

A repository-wide statement such as "the fork is improved" may describe added
capabilities, observability or validated engineering surfaces. A speed claim
must remain bound to the exact tested surface and receipt.

## Physical gap

GitHub-hosted CI cannot replace device receipts for ARMv7/AArch64 or independent
third-party reproduction. Those states remain TOKEN_VAZIO until observed.


## Physical device runner

`rmr/tools/run_physical_upstream_compare_v3.sh` is the canonical device-bound
runner for ARMv7, AArch64 and x86-64 Linux/Android environments.

It selects the same architecture contract for official and fork builds:

- ARMv7: NEON intrinsics + ARMv7/NEON flags;
- AArch64: NEON intrinsics + ARMv8-A;
- x86-64: native automatic dispatch;
- unsupported architectures: portable fallback.

The runner uses the same size harness and V2 statistical analyzer, records the
exact upstream/fork commits, compiler/CMake, architecture and non-unique CPU
capability fields, and preserves all CSV/JSON/SHA-256 receipts.

No physical state is promoted until that runner exits successfully on the
target device.
