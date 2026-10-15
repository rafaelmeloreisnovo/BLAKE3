# RMR / BLAKE3 — Typed capability and uncertainty gate (2026-10-15)

Status: SOURCE_GATED_CANDIDATE / BRANCH_DRAFT / NO_PROMOTE.
Purpose: disambiguate the official BLAKE3 primitive, fork build orchestration, RMR portable kernels, CF140 supervisor and Android demonstration. This is a source-side falsifier and a typed gap ledger, NOT new cryptographic functionality.

## Source authority / exact references
- Producer: rafaelmeloreisnovo/BLAKE3; base master `584a686ed68d3fb0189797a661884678a78df01b`.
- Android comparator candidate: PR #177 HEAD `46c5f88f073a7aafb062953bc56a2fe8e36089c5` (DRAFT; do not merge on this receipt).
- Pinned official candidate: BLAKE3-team/BLAKE3 `f55849f89cd85083c1c9daa2c5de20766f309c00`.
- RMR fixed256 implementation: `rmr/portable/src/rmr_portable_v1.c` (base blob `5a67d778d165be0c0c9f774354bbc461e71d2546`).
- BLAKE3 adapter: `rmr/portable/provider/blake3/rmr_portable_blake3_v1.c` (base blob `da409066bebc449c629cd82d7e12b4ce3f64ae64`).
- CF140 controller: `rmr/crypto_freestanding140/control/rmr_cf140_guard.c` (base blob `910ee6decb2b014a23ee07bac3284b03a7f833b4`).

## Type system (SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM)
| Capability / proposition | Typed state | Evidence boundary |
| --- | --- | --- |
| BLAKE3 hash / XOF / keyed and key-derivation modes | UPSTREAM_PRIMITIVE | Not an RMR invention; JNI triple benchmark exercises unkeyed digest only |
| Official, fork and adapter JNI paths | SOURCE_IMPLEMENTED_PR177_DRAFT | Three library/provider paths; fork and RMR adapter share the underlying C source |
| Triple KAT abc on ARM32 | USER_REPORTED_DEVICE_OBSERVATION | 3 digest values match known BLAKE3 abc; independent APK/.so attestation absent |
| 1MiB equality on ARM32 | USER_REPORTED_PASS | Input bytes/corpus and all three digest outputs not retained independently |
| 1MiB speed 2 rounds | EXPLORATORY_UNCONTROLLED | RMR relative to official +0.24% then -0.90%; no thermal or clock control, confidence interval or raw samples |
| 256->16 fixed XOR reducer, unrolled batch4 | AUTHORIAL_SOURCE_IMPLEMENTED_NONCRYPTO | Linear transformation, not collision resistant; counterexample test added here |
| C BLAKE3 wrapper provider | SOURCE_IMPLEMENTED_UPSTREAM_BACKED | Invokes upstream init/update/finalize; no novel BLAKE3 algorithm |
| Rust no_std and Java fixed-frame mirror | SOURCE_PRESENT_AND_SCOPE_LIMITED | Rust = ABI interface, Java = runtime hosted; neither proves full native Android/bare-metal execution |
| CF140 140 registry addresses | INDEX_REGISTERED_NOT_140_PRIMITIVES | One SHA-256 compression-only H01 core; registry presence != algorithm implementation |
| CF140 fail-ACK bounded retry/rollback | SUPERVISOR_SOURCE_IMPLEMENTED | Explicit event machine; no autonomous board watchdog or physical rollback |
| C/Rust freestanding target builds | HISTORICAL_SCOPED_CI_EVIDENCE | Link/rlib matrix evidence was recorded in earlier gates; does not prove ARM32 device bare-metal execution |
| Live official-vs-fork build matrix | SOURCE_IMPLEMENTED_STATISTICAL_HARNESS | Paired runner-local comparison; no universal 10-20x speed claim |
| Optional oneTBB acceleration | BUILD_OPTION_PRESENT | Target/config-specific; not proven active in Android triple JNI run |
| Chain of custody, source manifests | TOOLING_AND_PROTOCOL_PRESENT | Hashes and logs enable verification, but integrity alone is not authenticated authorship or tamper-proof storage |
| FiberHaga | SEPARATE_CORE_OUT_OF_SCOPE | Not the same as BLAKE3 fork engine |
| Android ARM64 same APK run | TOKEN_VAZIO_NOT_OBSERVED | Requires actual handset receipt and binary digest matching |
| Android physical device / APK signature / binaries | TOKEN_VAZIO_ATTESTATION | User export is observation only, no trusted hardware/provider attestation |
| Global promotion | HOLD_NO_PROMOTE | Separate producer exact-head CI, independent review, provider enforcement, device matrix |

## Source-side negative/falsifier gate (this branch)
`tests/selftest_reducer_noncrypto.c` constructs:
1. zero[256] versus two changes (byte 0 and byte 16), both reducing to the same zero[16];
2. independent single-byte mutations at byte 0 and byte 16, both producing the same nonzero 16-byte reduction.

A passing test proves a collision exists for the separate XOR reducer, and prevents a misleading cryptographic-security claim. It does NOT indicate a BLAKE3 collision, which is an entirely different operation. It does not change any BLAKE3 C/Rust/Android implementation.

Hosted gate in `build/build_host_selftest.sh` compiles with strict flags, runs this negative control and emits `RMR_FIXED256_NONCRYPTO_COLLISION_FALSIFIER=PASS` ONLY on success. Until exact-head CI completes, status=IMPLEMENTED_UNTESTED.

## Closeout
F_ok: typed boundary at public header + reproducible negative source-side falsifier + no cryptographic core changes.
F_gap: CI/exact head, upstream-vs-fork exact-source diff, device APK/.so hashes and signing, second ABI physical run, raw benchmarks, provider governance, final code review.
F_next: wait for bounded source gate CI on this exact PR head, repair only observed failures, independently verify Android PR #177/device artifact chain, then decide on promotion separately.
claim_allowed=false; no merges; rollback=close draft PR or revert ordinary commits; corrections append successor with parent.
