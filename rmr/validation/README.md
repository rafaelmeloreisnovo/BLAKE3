<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR BLAKE3 Full Validation Matrix V1

This directory defines the complete comparison campaign between official BLAKE3,
the modified fork, and the wider RMR system.

## Three distinct objects

```text
BLAKE3_UPSTREAM
!=
BLAKE3_FORK_RMR
!=
RMR_FULL_SYSTEM
```

A result is never promoted across these boundaries.

## Test axes

1. C known-answer vectors under ASan/UBSan.
2. Current upstream vs fork C-core multi-size V2.
3. C-source ablation: branch hints, restrict annotations and both together.
4. x86 backend matrix: portable, SSE2, SSE4.1, AVX2, native dispatch.
5. Rust crate tests.
6. Rust release-profile ablation: tuned LTO/codegen-units=1 vs untuned.
7. b3sum hashing workload.
8. b3sum checkfile workload, including the fork 64 KiB reader.
9. ELF/binary size, symbols, sections and non-executable GNU_STACK.
10. warning and sanitizer evidence.
11. cross-architecture compile contracts.
12. ARM physical receipts.
13. IOPS workload receipts.
14. provenance and claim gates.

## Interpretation rule

A source modification gets performance credit only when its ablation shows a
repeatable effect under the same harness. Repository-wide improvements such as
receipts, topology, legal/governance, HWIF and SIMPERF are documented as system
capabilities, not cryptographic throughput.

## Status labels

- PASS — observed successful execution.
- FAIL — observed failure.
- PENDING — scheduled but not completed.
- TOKEN_VAZIO_* — unavailable/unknown by design.
- REVIEW — evidence exists but does not support promotion.

No ranking or performance claim is inferred from source inspection alone.
