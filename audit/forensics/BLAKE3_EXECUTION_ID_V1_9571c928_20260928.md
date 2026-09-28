# BLAKE3 RMR — canonical execution receipt

**EXECUTION_ID:** `BLAKE3-RMR-EXEC-V1-9571c928808b-20260928T064421Z-99of99`  
**State:** `PASS_DOCUMENTARY_AND_CI`  
**Repository:** `rafaelmeloreisnovo/BLAKE3`  
**Execution commit:** `9571c928808b9aba96e7b9586c54c12ac66cdc3e`  
**Tree:** `a23db46482c2774c627f2ba12a46cead898db4d7`  
**Event:** `push` on `master`  
**Run creation time:** `2026-09-28T06:44:21Z`  
**First check start:** `2026-09-28T06:44:37Z`  
**Last check completion:** `2026-09-28T07:06:36Z`  
**Observed wall span:** `1319 s = 21m59s`

## 1. Commit custody

Merge commit:
`9571c928808b9aba96e7b9586c54c12ac66cdc3e`

Parents:
- `4358dfd56e7b26d8e5e5363820ca8edb493363ed`
- `c23a2ef590588044e75d4c7dabe11049ca2b114b`

Message:

`Merge pull request #162 from rafaelmeloreisnovo/hardening/rmr-action-pins-v1-20260928`

`Hardening: pin all fork-local RMR GitHub Actions to immutable SHAs`

GitHub commit verification observed: `verified=true`, `reason=valid`.

## 2. Terminal check state

The complete check-run collection for the execution commit was paginated with `per_page=100`.

```text
total_count = 99
returned    = 99
success     = 99
failure     = 0
cancelled   = 0
queued      = 0
in_progress = 0
```

Therefore:

`99/99 terminal checks = SUCCESS`.

The exact per-check ledger is frozen at:

`audit/forensics/BLAKE3_EXECUTION_CHECKS_V1_9571c928_20260928.tsv`

Git blob:

`687d8621bf01011d3a88b22bcd46689e6a8222ea`

## 3. Workflow-run topology

| Run ID | Workflow | Workflow path | Checks | Conclusion |
|---:|---|---|---:|---|
| 36387950893 | tests | `.github/workflows/ci.yml` | 74 | success |
| 36387951003 | RMR upstream comprehensive V3 | `.github/workflows/rmr-upstream-comprehensive-v3.yml` | 10 | success |
| 36387950926 | RMR full validation matrix | `.github/workflows/rmr-full-validation.yml` | 5 | success |
| 36387950997 | RMR crypto runtime matrix | `.github/workflows/rmr-crypto-runtime.yml` | 1 | success |
| 36387950918 | RMR BLAKE3 Reproducibility Gate | `.github/workflows/rmr-blake3-repro.yml` | 1 | success |
| 36387950942 | rmr-standalone-v1 | `.github/workflows/rmr-standalone-v1.yml` | 1 | success |
| 36387950960 | rmr-pure-core-boundary-v1 | `.github/workflows/rmr-pure-core-boundary-v1.yml` | 1 | success |
| 36387951049 | RMR hardware build topology | `.github/workflows/rmr-hardware-build-topology.yml` | 1 | success |
| 36387950917 | RMR ZIP Custody Gate | `.github/workflows/rmr-zip-custody.yml` | 1 | success |
| 36387950933 | RMR SIMPERF | `.github/workflows/rmr-simperf.yml` | 1 | success |
| 36387950923 | rmr-hash-boundary-v1 | `.github/workflows/rmr-hash-boundary-v1.yml` | 1 | success |
| 36387950968 | RMR BLAKE3 upstream compare V2 | `.github/workflows/rmr-blake3-upstream-compare-v2.yml` | 1 | success |
| 36387950902 | rmr-crypto-freestanding140-v1 | `.github/workflows/rmr-crypto-freestanding140-v1.yml` | 1 | success |

Total: **13 workflow runs / 99 checks**.

## 4. Executed workflow blobs

| Workflow path | Git blob SHA |
|---|---|
| `.github/workflows/ci.yml` | `f7dd8c053acc2c215032a17482273a2ca00d90e5` |
| `.github/workflows/rmr-full-validation.yml` | `0a6de14f7d0743a3882ea2276cda3d9d3636a75d` |
| `.github/workflows/rmr-crypto-runtime.yml` | `f6b516f39f5442296a868001451cb26e5c0fa3c5` |
| `.github/workflows/rmr-blake3-repro.yml` | `9bd64161e0dd0f6cffc639928d3e17649f1b0081` |
| `.github/workflows/rmr-standalone-v1.yml` | `0637b27353aade0a75db84536236901a9d52d613` |
| `.github/workflows/rmr-pure-core-boundary-v1.yml` | `27c524c696ad4a9589cd7ce8d457e3edbc1bf6e7` |
| `.github/workflows/rmr-hardware-build-topology.yml` | `16f1f75e0620b27456f95e46c05a5ee644ea9512` |
| `.github/workflows/rmr-zip-custody.yml` | `a2b6370d5b552f63150a7bf0f89cc2e36dcaa1a5` |
| `.github/workflows/rmr-simperf.yml` | `05e402e6df8a53bbf5f859ee86dbb32d569f441a` |
| `.github/workflows/rmr-hash-boundary-v1.yml` | `03e9f25863e53147ba213296e066623d8153ad18` |
| `.github/workflows/rmr-upstream-comprehensive-v3.yml` | `802db1dd890d2f648d62e550930ec9db92418f43` |
| `.github/workflows/rmr-blake3-upstream-compare-v2.yml` | `cf178097f5da1133e0653b01175f9a2ae88484f1` |
| `.github/workflows/rmr-crypto-freestanding140-v1.yml` | `1ad888eb238303798381197f0a42942638c660f2` |

This freezes the exact automation source used by the execution.

## 5. Artifact custody

The execution produced **20 persisted GitHub Actions artifacts** with GitHub-provided SHA-256 digests.

The canonical artifact ledger is:

`audit/forensics/BLAKE3_EXECUTION_ARTIFACTS_V1_9571c928_20260928.tsv`

Git blob:

`366af9753da1466f7b6e40b51f6bd9adb9fad574`

Examples of sealed artifacts:

| Artifact | SHA-256 |
|---|---|
| `rmr-full-validation-receipt` | `4473690616b72ef91ce698765ed19cce08ba831fb130ae2c907d26da3a5112fd` |
| `upstream-v3-final-receipt` | `a07e72fd331ca6b743401f3a9bc139637e26c75ed8ffb04165c056019e7b1837` |
| `rmr-blake3-repro-receipts` | `c99e7a0d1c943996c592ff1ccc38a94cb75c5c6eb0bb835c4612147aabdfbdff` |
| `rmr-crypto-runtime-matrix` | `54e574f66e713c08630a1106d62e6cb1529bd354d42edc6beb45ad4189351630` |
| `rmr-hardware-build-topology` | `637ce741fc085726be9c53427c386aa23c3c73a99ed762140604435b7c603154` |
| `rmr-simperf-receipts` | `230ae4406d97fa9f0a41ded1f764f1ad36592284b1c9d933ca0e88d434464c9a` |
| `rmr-blake3-upstream-compare-v2` | `c10e509c41bb3659efd1071d7e3896660ba522294273d6c6d794d4d9d4ca4fd8` |

The TSV preserves all 20 artifact IDs, sizes, digests, timestamps and expiry timestamps.

## 6. Runtime/toolchain evidence

### RMR Ubuntu jobs

Observed runner image reference in representative RMR jobs:

`actions/runner-images ubuntu24/20260920.314 / Ubuntu2404`

Observed pinned action objects:

```text
actions/checkout v6
d23441a48e516b6c34aea4fa41551a30e30af803

actions/checkout v4
11d5960a326750d5838078e36cf38b85af677262

actions/upload-artifact v4
ea165f8d65b6e75b540449e92b4886f43607fa02

actions/download-artifact v4
d3f86a106a0bac45b974a628896c90dbdf5c8093

actions/setup-python v6
ece7cb06caefa5fff74198d8649806c4678c61a1
```

A runtime emitted a warning that selected actions still target Node.js 20 and were **forced by the runner to Node.js 24**. The action Git objects remained pinned; the Node runtime boundary is recorded as environment behavior.

### Crypto runtime

Observed:

```text
Python 3.12.3
CMake 3.31.6
RMR_CRYPTO_RUNTIME_REGISTRY=PASS
RMR_CRYPTO_ALGORITHM_LAYOUT=PASS algorithms=28
RMR_CRYPTO_CONTRACT_MATRIX=7/7
RMR_CRYPTO_CARTESIAN=PASS cells=252
RMR_CRYPTO_MATRIX_CELLS=252/252
RMR_CRYPTO_NATIVE_CELLS=28/28
```

`OPENSSL_CROSS_PROVIDER=TOKEN_VAZIO_PROVIDER_TOOLCHAIN`.

### Freestanding140

Observed:

```text
Ubuntu clang 18.1.3
Ubuntu LLD 18.1.3
CF140_HOST_SELFTEST=PASS
CF140_SHA256_ABC_COMPRESSION_KAT=PASS
CF140_SOURCE_SHAPE=PASS
CF140_FREESTANDING_ARTIFACT=PASS
```

Produced freestanding binary hashes:

```text
x86_64
a62dc0da8ba56126cd2b031e7398ae67c0c55be02c4b04b540cc88d2fa5d815a

aarch64
82d4d557aafd550d137f03321ac63393c9521ff91b33fef07c75925bf7c3c798

armv7
22a6abfb223c6aac8b66e57a2e6ab474c777692ae026ac17a4b51e196151e544
```

### Official upstream-identical CI

Representative observed runtime facts:

```text
Ubuntu runner image: ubuntu-24.04 / ubuntu24/20260920.314
actions/checkout@v4 runtime object:
  11d5960a326750d5838078e36cf38b85af677262

dtolnay/rust-toolchain@master runtime object:
  02cb101ec7c40f2c49e1d9714d64511d8e1b74de

stable:
  rustc 1.98.1 (48a229cea 2026-09-01)
  cargo 1.98.1 (797e8a9bc 2026-08-05)

beta:
  rustc 1.99.0-beta.8 (dfb0accb7 2026-09-26)

WASM:
  rustc 1.98.1
  target wasm32-wasip1
  Wasmtime v49.0.1
```

Exact versions for every one of the 74 matrix cells are not independently enumerated in this document; their check IDs and timestamps are frozen in the 99-check ledger.

## 7. Aggregate evidence

### Full validation

Observed terminal state:

```text
RMR_FULL_VALIDATION_TERMINAL_STATE=COMPLETE
RMR_VALIDATION_COMPLETENESS_SELFTEST=PASS
RMR_FULL_VALIDATION_EXECUTION_COMPLETE=true
RMR_FULL_VALIDATION_STATE=COMPLETE
RMR_FULL_VALIDATION_EVIDENCE_GATE=PASS
```

Components:

```text
C-KAT-SAN=PASS
C-UPSTREAM-V2=PASS
C-ABLATION=PASS
SIMD-X86=PASS
RUST-LTO=PASS
B3SUM=PASS
BINARY=PASS
CROSS-ARCH=PASS
```

Explicit limits:

```text
ARM-PHYSICAL=TOKEN_VAZIO_PHYSICAL
IOPS-PHYSICAL=TOKEN_VAZIO_CURRENT_CAMPAIGN
INDEPENDENT-REPRODUCTION=TOKEN_VAZIO
```

### Upstream comprehensive V3

Observed:

```text
RMR_UPSTREAM_COMPREHENSIVE_TERMINAL_STATE=COMPLETE
RMR_UPSTREAM_COMPREHENSIVE_EXECUTION_COMPLETE=true
RMR_UPSTREAM_COMPREHENSIVE_STATE=COMPLETE
RMR_UPSTREAM_COMPREHENSIVE_EVIDENCE_GATE=PASS
RMR_UPSTREAM_COMPREHENSIVE_V3=PASS
```

Components:

```text
SOURCE-PROVENANCE=PASS
C-QUALITY=PASS
C-CORE-PERFORMANCE=PASS
C-ABLATION=PASS
SIMD-BACKENDS=PASS
C-TBB=PASS
ABI-ELF=PASS
RUST-LIBRARY=PASS
B3SUM-CLI=PASS
CROSS-ARCH=PASS
RMR-INTEGRATION=PASS
```

Important non-green semantic detail preserved inside a PASS framework:

`C-CORE-PERFORMANCE` contained seven size classes and at least one classification
`OBSERVED_SLOWER_ON_THIS_RUNNER`.

Therefore `PASS` is interpreted as **the validation contract completed successfully**, not as “the fork was faster in every size class”.

Explicit limits remain:

```text
ARM-PHYSICAL=TOKEN_VAZIO_PHYSICAL
INDEPENDENT-REPRODUCTION=TOKEN_VAZIO
```

## 8. Reproducibility-gate observation

The reproducibility workflow reported:

```text
status=PASS
rmr_median_mib_s=3978.110
rmr_over_official_ratio=0.997008x
independent_third_party_reproduction=TOKEN_VAZIO
```

It also emitted compiler warnings for implicit fallthrough in `rmr/include/rmr_lowlevel.h`; these warnings did not make the workflow fail and are retained as an engineering gap rather than suppressed.

## 9. Claim gate

Permitted by this receipt:

```text
EXECUTION_COMMIT_FIXED=true
WORKFLOW_BLOBS_FIXED=true
CHECK_COLLECTION_COMPLETE=true
CHECKS_SUCCESS=99/99
WORKFLOW_RUNS_SUCCESS=13/13
ARTIFACT_DIGESTS_CAPTURED=20/20
FULL_VALIDATION_COMPLETE=true
UPSTREAM_COMPREHENSIVE_COMPLETE=true
CRYPTO_MATRIX=252/252
CRYPTO_NATIVE=28/28
FREESTANDING_X86_64_PASS=true
FREESTANDING_AARCH64_BUILD_PASS=true
FREESTANDING_ARMV7_BUILD_PASS=true
```

Not promoted:

```text
ARM_PHYSICAL_EXECUTION=TOKEN_VAZIO
AARCH64_PHYSICAL_EXECUTION=TOKEN_VAZIO
INDEPENDENT_THIRD_PARTY_REPRODUCTION=TOKEN_VAZIO
PHYSICAL_IOPS=TOKEN_VAZIO_CURRENT_CAMPAIGN
EVERY_TOOLCHAIN_BINARY_DIGEST=TOKEN_VAZIO
UNIVERSAL_PERFORMANCE_SUPERIORITY=NOT_CLAIMED
```

## 10. Canonical reconstruction rule

To reconstruct this execution identity, a verifier needs:

1. repository `rafaelmeloreisnovo/BLAKE3`;
2. commit `9571c928808b9aba96e7b9586c54c12ac66cdc3e`;
3. the 13 workflow blob SHAs listed above;
4. all 99 check-run rows from the check ledger;
5. all 20 artifact digest rows from the artifact ledger;
6. terminal aggregate evidence and explicit `TOKEN_VAZIO` states.

The human-readable identifier is:

`BLAKE3-RMR-EXEC-V1-9571c928808b-20260928T064421Z-99of99`

It is an address for this evidence snapshot, not a substitute for the underlying Git/SHA-256 objects.

## R3

`F_ok`: 13/13 workflows success; 99/99 checks success; 20/20 persisted artifacts carry SHA-256 digests; executed workflow blobs frozen; commit signature valid; runtime/toolchain evidence recorded.  
`F_gap`: ARM/AArch64 physical execution, physical IOPS and independent third-party reproduction remain TOKEN_VAZIO; upstream CI still contains moving references even though runtime-resolved action/toolchain objects were observed; implicit-fallthrough warnings remain in RMR low-level code.  
`F_next`: perform an independent device-bound ARMv7/AArch64 reproduction against this EXECUTION_ID and append a successor receipt without mutating this snapshot.
