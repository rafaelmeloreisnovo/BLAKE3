<!--
Copyright (c) 2026 Rafael Melo Reis.
Licensed under rmr/LICENSE_RMR.
-->
# RMR BLAKE3 Android Laboratory V1

State: SOURCE_MATERIALIZED / APK_BUILD=NOT_RUN / DEVICE_EXECUTION=TOKEN_VAZIO.

An independently packaged Android research APK inside the BLAKE3 fork's `rmr/` boundary.
Design input: the Java/JNI/freestanding C split observed in
[EstudioAudio](https://github.com/rafaelmeloreisnovo/EstudioAudio).
No EstudioAudio source code is incorporated.

## Actual composition

```text
app/src/main/java/            Java platform UI + fail-closed gate
app/src/main/cpp/rmr_jni_bridge.c   hosted JNI translation
rmr/portable/provider/blake3/      RMR adapter/memory shim
c/                                upstream BLAKE3 C provider (unchanged)
```

- Supported packaging: `armeabi-v7a` and `arm64-v8a`; minSdk 29.
- One-shot BLAKE3-256 over a maximum of 1,048,576 UTF-8 bytes.
- Built-in KAT: `BLAKE3("abc") = 6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85`; empty input = `af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262` (cross-checked against the repository's upstream vectors).
- No network, storage, microphone, camera, or sensor permission.
- No AndroidX, Kotlin, JavaScript runtime, dynamic provider selection, or DSP dependency.
- All algorithmic C data are caller-owned, stack-local, or provider-local.
- Provider sources and cryptographic semantics retain upstream attribution.
- JNI and the final Android .so are platform-linked. **They are not freestanding executables**.
- The RMR provider pure-link freestanding gate lives in `rmr/portable/build/build_cross_matrix.sh`, not in the Android package.
- Uses external Android SDK/NDK, JDK, Gradle and official Android plugin **only for building and running an Android APK**.

## Build

From this repository root with Android SDK/NDK, CMake, Gradle 8.11.1 and Java 17:

```sh
sh rmr/portable/build/build_host_selftest.sh
cd rmr/android
RMR_SOURCE_SHA="$(git -C ../.. rev-parse HEAD)" gradle --no-daemon :app:assembleDebug
```

APK: `rmr/android/app/build/outputs/apk/debug/app-debug.apk`.

CI: `.github/workflows/rmr-android-apk.yml` builds with explicitly bounded environment,
checks ABI entries, hashes APK, publishes the debug artifact and a scoped text receipt.
A successful CI build cannot prove physical installation, execution, or device-side digest.

## Source / artifact / execution / evidence

| Claim | Required evidence |
| --- | --- |
| RMR provider source exists | exact Git SHA and source path |
| upstream digest equivalence | vector KAT at same head |
| provider pure-link is freestanding | freestanding ELF readelf/nm gate |
| APK packs both ABIs | ZIP inventory of built APK |
| device executes digest | physical same-APK + device + JNI receipt |
| release is production-ready | reproducible signed release + audit + device matrix |

Unmet evidence is `TOKEN_VAZIO`, not PASS. Debug signing is not distribution signing.

## Next bounded integration

A future **separate** EstudioAudio consumer may hash its already-owned PCM/ZIPRAF
buffers through an exact-SHA pinned RMR provider interface. This APK does
not include recording, DSP, file hashing, app-to-app integration, or acoustic claims.

Rollback: close this PR or revert `rmr/android/**` and its dedicated workflow.
No changes to upstream `c/` or EstudioAudio are necessary.
