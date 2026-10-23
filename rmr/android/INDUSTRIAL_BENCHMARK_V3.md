<!-- Copyright (c) 2026 Rafael Melo Reis; rmr/LICENSE_RMR. -->
# RMR Android BLAKE3 Industrial Benchmark V3 — auditable, scoped, nonpromotable

## Baseline and authority
This is an **additive research mode** in the existing offline three-provider APK.
The old V1 4 KiB/1 MiB buttons, native JNI ABI, KATs, upstream algorithms,
fork C sources, RMR portable implementation and all existing license assets
are unchanged. V3 orchestration is Java/platform-hosted. It does not establish
bare-metal execution or a new cryptographic hash function.

Producer: rafaelmeloreisnovo/BLAKE3, rmr/android/.
Official checkout must be exactly
BLAKE3-team/BLAKE3@f55849f89cd85083c1c9daa2c5de20766f309c00.
The fork identity is recorded by BuildConfig.SOURCE_SHA, populated by
exact-HEAD CI; TOKEN_VAZIO means provenance is missing, never zero.

## Fixed measurement protocol
- Identical deterministic byte payload per size: byte[i] = (i*29+7) mod 256.
- Five sizes in **interleaved order**: 4 KiB, 1 MiB, 16 KiB, 256 KiB, 64 KiB.
- Three blocks/size; each block: 4 full three-provider warmup rounds plus
  9 full three-provider measurement rounds.
- Ordering for each slot: provider index (block*13+round+slot)%3 (zero-based).
  Every provider occurs in each slot 13 times over the full 39 rounds/size.
- 27 raw timed values per provider/size, **405 total retained samples**.
  Every measurement calls System.nanoTime() on either side of one digest
  invocation including Java/JNI and byte-array work. It does not measure
  isolated BLAKE3 compression throughput, and the three sequential
  measurements are not simultaneous.
- Validate official/fork/RMR digest before, during every warmup and measured
  call, and after each size. Compare against the same 32-byte reference.
  Detect input-buffer mutation; reject null, partial and invalid digests.
- No sample omission. Export ordered, unrounded RAW_NS_size_provider,
  median, raw median absolute deviation (MAD), nearest-rank p95 and
  first/last block medians. Median ratios are descriptive and do not
  supply confidence bounds or a statistical claim.

## Observed environmental metadata, NOT active controls
Android SDK >= 29: PowerManager.getCurrentThermalStatus();
battery-temperature broadcast (tenths °C; battery, not core temperature);
elapsedRealtimeNanos and untrusted wall clock at each size boundary.
The app additionally reports whether its process is 32 or 64 bit and the
corresponding supported ABI candidate (not independently checked ELF ISA).
No CPU governor, pinning, frequency, cooling, charging or background-load
lock is claimed. Unavailable measurements carry typed TOKEN_VAZIO.
The physical result remains ON_DEVICE_OBSERVATION_NOT_PROVIDER_ATTESTATION.

## Fail-closed and regression boundaries
KATs (official empty/abc, RMR self test, five boundary equalities) must pass
before the V3 button becomes active, as in V1. Any invalid time, absent
dependency, unequal digest, mutation or incomplete sample interrupts the
campaign and emits STATUS=BLOCKED_FAIL_CLOSED. Partial measurements are
not promoted as completed results. Output CLAIM_ALLOWED=false always.

No signature or provider attestation is added by sharing text.
The user must bind a physical device receipt to the CI-built APK SHA256.
Debug APK signatures are install signatures, not releases.

## Evidence gates and remaining uncertainty
| Gate | Evidence | State |
|---|---|---|
| Hosted orchestration determinism, raw samples, negative falsifiers | BenchmarkCampaignV3Test fake clock | TESTABLE_IN_CI, not device proof |
| Real three-backend digest equality | Native/Android KAT gates, then each size | NEEDS_DEVICE_RUN |
| Android Java/native build | rmr-android-apk.yml exact-head | NEEDS_EXACT_HEAD_CI |
| ARM32 handset + ARM64 handset | Same APK, receipts, APK hashes | TOKEN_VAZIO |
| Thermal/frequency stabilization | Controlled repeats/instrumentation | NOT_PERFORMED |
| Statistical separation and crossover | Repeated campaigns, paired/block analysis, bootstrap CI | NOT_COMPUTED |
| Independent cryptographic/license review and provider gate | Third-party evidence | TOKEN_VAZIO |

**Physical-run acceptance proposal:** at least three independent campaigns
per device and per ABI/profile, preserve all raw samples, capture charging
state and battery/thermal readings, and analyze paired blocks with uncertainty
bounds. Do not pool ARM32 with ARM64, attribute thermal causation without a
control, or treat individual samples as independent campaign replicates.

## R3 and rollback
F_ok = additive V3 source and deterministic hosted falsifiers when tested;
F_gap = provider CI / Android physical run / clock controls / independent audit;
F_next = exact-head CI + same-APK dual-ABI on-device receipts with raw values.

Rollback: close the unmerged draft PR; no main/core deletion or mutation.
SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM.
TOKEN_VAZIO != 0; claim_allowed=false.
