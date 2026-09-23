<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR upstream validation V3

This directory is the canonical test map for comparing
`BLAKE3-team/BLAKE3` with the Rafael/RMR fork.

The suite deliberately does not reduce the repository to one benchmark.

## Surfaces

1. C correctness and official test vectors.
2. C core performance by message size.
3. x86 dispatch/backend ablation.
4. ARM NEON compile and physical-receipt boundary.
5. Strict diagnostics.
6. ASan/UBSan.
7. ABI, symbols, ELF sections, binary size and GNU_STACK.
8. CMake install/pkg-config consumer compatibility.
9. Rust library tests/build.
10. Fork Rust LTO/codegen-unit ablation.
11. b3sum CLI/file/checkfile behavior.
12. Cross-architecture C compile matrix.
13. WASM Rust compile.
14. Full RMR integration gates.
15. Source/provenance classification.
16. Independent physical reproduction.

Every output is scoped. A PASS in one surface is not inherited by another.

## Claim rule

```text
SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM
CORE_GAIN != CLI_GAIN
X86_GAIN != ARM_GAIN
```

A performance claim requires a receipt with commit, compiler, flags, hardware,
workload, repetitions, dispersion and the common benchmark source.

Physical ARM/AArch64 performance remains TOKEN_VAZIO until a device actually
executes the matching runner.
