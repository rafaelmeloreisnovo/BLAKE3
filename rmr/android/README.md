<!-- Copyright (c) 2026 Rafael Melo Reis; rmr/LICENSE_RMR. -->
# RMR BLAKE3 Android Three-Provider Comparator V2

Status: SOURCE_CANDIDATE / BUILD_AND_NATIVE_PKG_GATED / DEVICE_PHYSICAL_TOKEN_VAZIO / NO_PROMOTE.

**One Android APK contains both `armeabi-v7a` and `arm64-v8a`.** Each
installed copy selects its ABI via Android's standard package loader.
The three buttons/providers are compared ON THE SAME ABI and SAME DEVICE.
The same APK must be separately installed and observed on each 32/64-bit
device to support cross-device claims. Do not pretend an APK simultaneously
executes both ABIs in one process.

| Path | Sources | Native library | Boundary |
|---|---|---|---|
| Official BLAKE3 | BLAKE3-team/BLAKE3 exact SHA f55849f89cd85083c1c9daa2c5de20766f309c00 | `libofficialhash.so` | Unchanged upstream C files |
| Fork root | This repository's root `c/` at exact PR head | `libforkhash.so` | Fork C implementation |
| RMR | Existing `rmr/portable/provider/blake3/` adapter using fork root `c/` | `librmrhash.so` | RMR adapter, upstream algorithm |

**Not three hash algorithms:** there are three binary/provider paths to
the same BLAKE3 digest; RMR reuses the fork source and adds the adapter.
The existing RMR build flags, kernel and freestanding boundaries are
UNMODIFIED. No freestanding conversion of the official or fork cores.
JNI and Android libraries remain platform-hosted.

## Local build (Java 17, Android SDK 36, NDK 27.2, CMake 3.22.1, Gradle 8.11.1)

From repository root:
```sh
git clone https://github.com/BLAKE3-team/BLAKE3.git rmr/android/upstream_official
git -C rmr/android/upstream_official checkout --detach f55849f89cd85083c1c9daa2c5de20766f309c00
test "$(git -C rmr/android/upstream_official rev-parse HEAD)" = f55849f89cd85083c1c9daa2c5de20766f309c00
export BLAKE3_OFFICIAL_DIR="$PWD/rmr/android/upstream_official"
export B3_UPSTREAM_SHA=f55849f89cd85083c1c9daa2c5de20766f309c00
export RMR_SOURCE_SHA="$(git rev-parse HEAD)"
gradle --project-dir rmr/android --no-daemon :app:assembleDebug
```
Result: `rmr/android/app/build/outputs/apk/debug/app-debug.apk`.
The GitHub workflow `.github/workflows/rmr-android-apk.yml` does the
same pinned checkout in CI and checks the ZIP inventory, both ABI
directories, three libraries per ABI, official license assets and APK
debug signature. The DEBUG signature is installation-only, NOT authorial
release signing.

## Additive industrial campaign V3 (research, opt-in)

The original two benchmark buttons remain available and unchanged. The new
"Campanha industrial V3" button captures all 27 raw timings per provider and
size across a balanced 3-block rotation for 4, 16, 64, 256 and 1024 KiB.
It includes scoped equality checks, median/MAD/p95 and environment snapshots,
but DOES NOT control thermal/CPU clocks or attest the device. See
[INDUSTRIAL_BENCHMARK_V3.md](INDUSTRIAL_BENCHMARK_V3.md) for the protocol,
negative-test gates, unknowns and rollback. Claim allowed remains false.

## On-device laboratory protocol

1. APK displays actual ABI/Android API/phone model, exact fork and upstream SHAs.
2. Before enabling comparison, every provider must pass empty and `abc`
   known-answer tests and 5 cross-provider boundary inputs.
3. Text comparison prints all three BLAKE3-256 digests. Any mismatch
   yields `FAIL_CLOSED`.
4. Local benchmarks use the identical 4 KiB or 1 MiB buffer, 4 warmups,
   9 timing samples/provider, rotating provider order; prints per-provider
   median `System.nanoTime` with JNI overhead and time ratios to
   the official baseline. No universal speed claims.
5. Export/share the text receipt via standard Android Sharesheet.
   This is a local measurement, not a cryptographically signed or
   provider-attested execution receipt.

No network/SAF permissions and no private Drive/GitHub corpus. The
APK bundles readable full license texts and an attribution screen.

F_ok: reused existing Android project without altering provider
algorithm sources and bounded the three requested paths.
F_gap: physical execution on both ARM32/ARM64 devices; systematic
thermal/clock/ABI profiling; independent legal/crypto review.
F_next: exact-head CI + download and install same APK on both devices,
check native library/class/ABI source, save device-bound receipts.
