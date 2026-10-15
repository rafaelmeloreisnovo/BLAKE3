# RMR BLAKE3 — pure compile-policy delta V1

Status: `SOURCE_IMPLEMENTED / CI_EXACT_HEAD_NOT_YET_ATTESTED / CLAIM_ALLOWED=false`.
Producer: `rafaelmeloreisnovo/BLAKE3`.
Base `master@584a686ed68d3fb0189797a661884678a78df01b`.
Reference official: `BLAKE3-team/BLAKE3@f55849f89cd85083c1c9daa2c5de20766f309c00`.
No BLAKE3 cryptographic source was edited by this experiment.

## Reproducible question

What is the measurable difference in **pure BLAKE3 hash throughput and binary build output**, when comparing (a) upstream official versus fork source, and (b) the fork's own compile policy versus default Cargo release?

The existing V4 cross-source C matrix enforces the same manually selected flags for both C sources. It identifies differences under controlled configuration but cannot, by itself, answer a question about **each project's native build policy**. The Android triple JNI test includes platform overhead and cannot answer this question either.

## Why a 2x2 Rust build experiment is necessary

`build.rs` is byte-identical at the pinned sources (blob `7f4520e512e0b02b56456acf0171e3085d433c18`).

However the fork's `Cargo.toml` adds:

- `[profile.release] lto = true; codegen-units = 1`
- `[profile.bench] lto = true; codegen-units = 1`

Thus compare **four** controlled cases:

| Case | Code | Release policy |
| --- | --- | --- |
| `official_rust_native` | official | stock/default |
| `official_rust_tuned` | official | forced LTO + one codegen unit |
| `fork_rust_stock` | fork | forced stock LTO=off and 16 codegen units |
| `fork_rust_native` | fork | its own tuned profile |

For each input size and paired alternating round, calculate:

- `fork_rust_native / official_rust_native`: operational default difference (source **plus** policy).
- `fork_rust_stock / official_rust_native`: source difference at equal stock policy.
- `fork_rust_native / official_rust_tuned`: source difference at equal tuned policy.
- `official_rust_tuned / official_rust_native`: policy effect for official source.
- `fork_rust_native / fork_rust_stock`: policy effect for fork source.

All ratios use median paired throughput = baseline elapsed nanoseconds / challenger elapsed nanoseconds, seven alternating observations, with bootstrap 95% interval. Do not claim generic speed superiority from runner-local noise.

**Important limitation:** Cargo LTO affects the Rust executable/build, but C backends may be compiled separately by `cc` without whole-program C LTO. Capture exact build logs. The experiment must not be described as proving identical machine code or whole-program C optimization.

## C build question

Run two **native CMake** configurations with no extra RMR/JNI provider, no TBB and standard release, allowing each side to use its own `c/CMakeLists.txt`. Capture complete CMake compile commands (`compile_commands.json`), raw logs, static library hashes/sizes, and common C bench results.

`c/blake3.c` and `c/blake3_portable.c` are identical at the pinned comparisons. The fork dispatch adds `BLAKE3_LIKELY` branch prediction hints; this alone does not establish an actual speed difference. Also capture CMake version/compilers; this is distinct from the symmetric-flag V4 C source control.

## Evidence chain / no overclaim

`SOURCE -> BUILD_FLAGS/LOCK -> BINARY -> HOST_EXECUTION -> RAW_SAMPLES -> ANALYSIS -> RECEIPT`

Per exact run: source SHAs, source tree IDs, common harness SHA, compiler identity, Cargo.lock parity, build verbose logs, C compile commands, binary SHA-256/bytes, result CSV, digest equality across all scenarios and both languages, 95% CI, summary JSON/Markdown, receipt SHA manifest.

Hosted timer/benchmark harness is permitted for measurement, **not part of the pure hash**. An actual physical ARM32/ARM64 test is **not** produced by the Linux runner. A matched digest proves semantic compatibility for tested inputs, not compiler parity.

No attempt to rename Fork Root C, RMR adapter or FiberHaga as a novel BLAKE3 algorithm; no source licenses changed.

## Gates

- `SOURCE_PIN`: exact Git checkout official and exact workflow HEAD fork; branch drift aborts.
- `DEPENDENCY_RESOLUTION`: both ephemeral lock files must compare byte-for-byte or fail.
- `PROFILE_2X2`: compile all four distinct Rust cases; source and policy separately measured.
- `C_NATIVE`: independently configure/build both CMake sources; command diff captured rather than discarded.
- `PAIRING`: every variant, size and round required; duplicate or missing is failure.
- `DIGEST_EQUAL`: all six scenarios must emit the identical cryptographic BLAKE3 digest for each size.
- `STATISTICAL_SCOPE`: ratio, interval and local classification; no universal claims.
- `PHYSICAL_DEVICE`: `NOT_RUN`, not `PASS`.
- `CI_EXACT_HEAD`: `PENDING` until workflow run and artifacts are inspected.
- `PROMOTE`: `BLOCKED` pending CI, reviewer and provider authority.

## R3

F_ok: real source/build distinction documented; source-side runner, parser and deterministic negative tests added under dedicated draft branch.

F_gap: live toolchain execution, profile artifact hashes, command diff result, ARM32/ARM64 reproduction, independent review and exact-head CI/provider enforcement.

F_next: observe CI for the actual PR head, repair only concrete failures, preserve failure logs and no-promote title, then publish measured per-size deltas with recorded source, compiler, flags and device scope.
