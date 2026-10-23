# RMR — Promotion Gate Matrix V1

Scope: audit instrumentation in Draft PR #182. Baseline: master SHA ee7da2db2c9465d5c1ff9dcecf5435d5d3258973.

## Semantics
- Zero (0) is a measured quantity; it is never a substitute for unknown.
- TOKEN_VAZIO_<TYPE> = missing evidence/authority, not false, not zero.
- NOT_RUN = execution not observed; FAIL = observed failure.
- PASS_SCOPED applies solely to an exact source SHA, toolchain, target and evidence.
- COMMIT_TO_DRAFT != MERGE_TO_MASTER != CRYPTO_RELEASE != PERFORMANCE_CLAIM.
- This PR must not modify any preexisting code, comments, names, headers, timestamps or original build flags.

| Gate | Initial state | Falsifier / closure | Accountable authority |
|---|---|---|---|
| G0 baseline identity | VERIFIED_REPO_METADATA | pin HEAD and base commit in workflow | repository custodian |
| G1 additive-only changes | DIFF_SCOPED_OBSERVED | CI rejects changed/deleted original paths and paths outside allowlist | CI maintainer |
| G2 byte/name/ASCII/header/size/date | NOT_RUN_EXACT_HEAD | compare raw bytes with HEAD blobs, preserve filename hex, hash header prefix | CI |
| G3 void and warnings | NOT_RUN_EXACT_HEAD | lexical census and actual -Wshadow/-Werror compiler output, no inference from token counts | compiler reviewer |
| G4 semantic non-regression | NOT_RUN_EXACT_HEAD | existing hosted KAT/selftests and fixed256 noncrypto negative case | CI |
| G5 ELF closure | NOT_RUN_EXACT_HEAD | no PT_INTERP, DT_NEEDED or unexpected undefined symbols in each ELF | linker reviewer |
| G6 same-environment two-pass ELF identity | NOT_RUN_EXACT_HEAD | six pairs compare bit-for-bit and write hashes/sections/symbols | CI |
| G7 across-run reproducibility | TOKEN_VAZIO_CROSS_RUN | repeat on independent environment, pinned toolchain and compare exact hashes | independent reviewer |
| G8 physical ARMv7 | TOKEN_VAZIO_PHYSICAL_ARMV7 | exact artifact hash + device ABI/SDK + physical KAT receipt | device owner |
| G9 physical AArch64 | TOKEN_VAZIO_PHYSICAL_AARCH64 | exact artifact hash + device ABI/SDK + physical KAT receipt | device owner |
| G10 performance/superiority | TOKEN_VAZIO_STATISTICAL_BENCHMARK | controlled workload, repetitions, environment, raw times and uncertainty | performance reviewer |
| G11 independent review | TOKEN_VAZIO_HUMAN_REVIEW | approval after reading source and evidence; no automation can self-approve | human reviewer |
| G12 provider authority | BLOCKED_PROVIDER_RULESET | master protected=false was observed; enforce check+review or type provider incapacity and separate decision | repository owner |
| G13 release claim | BLOCKED_RELEASE_EVIDENCE | release-specific correctness, security, physical and human approvals | security + owner |

## Promotion lanes
- SOURCE COMMIT (permitted now): additive auditor and documentation commits on draft branch, without PASS promotion.
- MERGE (not yet permitted): exact-HEAD green CI, original files unchanged, independent review and provider policy enforcement; if GitHub cannot enforce this, explicitly mark TOKEN_VAZIO_PROVIDER_CAPABILITY and obtain separate human risk decision.
- CLAIM/RELEASE (not yet permitted): independent technical evidence and release criteria. A merge never promotes a scientific or cryptographic claim.

## Typed counterexamples
- Two different final ELF hashes -> FAIL_BINARY_REPLAY, never zero or unknown.
- A measured empty file -> bytes=0.
- No Actions check -> NOT_RUN_EXACT_HEAD, never PASS.
- No undefined symbols in inspected executable -> unexpected_und=0 IN THAT ELF ONLY.
- Lexical void count 0 -> void_lexemes=0; says nothing about effective ABI.
- Header/comment bytes changed -> SOURCE_BYTES_CHANGED; linker causality remains TOKEN_VAZIO until object/ELF comparison.
- A warning observed -> record exact compiler diagnostic; never alter original file merely to make it green.

Rollback: close Draft PR or revert additive commits; never delete original authorship or past receipts.
