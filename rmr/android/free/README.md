# RMR ZIPRAF FREE — Android local (ZRF1 experimental)

**Producer:** rafaelmeloreisnovo/BLAKE3, isolated module rmr/android/free/.
**Status:** CODE_CANDIDATE, NOT RELEASED. No historical upstream, rmr/portable or three-provider :app source modified.

## Product

One APK with two ABIs (armeabi-v7a + arm64-v8a), one native librmrfree.so per ABI, selected by the Android loader. Storage Access Framework handles file import/export without network or broad storage permission. Android Java/JNI are platform-hosted. C codec functions own no heap, filesystem or libc calls; underlying BLAKE3 remains upstream-origin code, not a new cryptographic algorithm.

Hard budgets enforced in GitHub CI:
- Native library per ABI <= 136 KiB = 139264 bytes.
- Whole APK <= 3 MiB = 3145728 bytes.
- Original local input <= 3 MiB; encoded file can add 46 header bytes.
- Debug signing is not publisher release signing.
These are targets, not observed measurements until the exact build returns a receipt.

Build with Java 17, Android SDK 36, NDK 27.2, CMake 3.22.1 and Gradle 8.11.1:

    gradle --project-dir rmr/android --no-daemon :free:assembleDebug

Expected output: rmr/android/free/build/outputs/apk/debug/free-debug.apk.

## ZRF1 versioned format

This is a new bounded, experimental ZIPRAF-FREE profile. **It is not claimed to interoperate with private ZIPRAF_CORE or ZIPRAF_OMEGA_FULL formats.** No private implementation is copied into this public repository.

46-byte header:
- offset 0..3: ASCII ZRF1
- offset 4: format version 1
- offset 5: mode 0 raw, 1 RLE
- offset 6..9: little-endian original length uint32, <= 3145728
- offset 10..13: little-endian encoded payload length uint32
- offset 14..45: BLAKE3-256 digest of uncompressed original bytes
- offset 46 onward: raw or RLE payload; no trailing extension

RLE: byte with high bit clear encodes (token & 127) + 1 literal bytes; byte with high bit set encodes (token & 127) + 3 copies of the next byte. Encoder chooses repeated runs of 4+ and falls back to raw when RLE is not smaller. Decoder rejects truncated payload, overflow, length mismatch, unknown version/mode and BLAKE3 digest mismatch. Incompressible files may grow by the 46-byte header.

BLAKE3 digest only verifies content integrity against accidental corruption or unauthorized changes *when the expected digest is trusted*. The embedded unkeyed digest does **not** prove origin, authenticated updates or authorship. No encryption, keyed MAC, signing, ZIP multi-file archive or legacy-format compatibility is claimed.

## Gates / provenance

1. Host known-answer tests for BLAKE3 empty/abc using the fork c/ files, deterministic reversible tests 0..3 MiB, corruption/truncation/malformed boundaries.
2. Dual ABI NDK link of one native library each; Java entrypoint compiles.
3. APK inventory, license notices, byte-size budgets, zipalign and debug apksigner verification.
4. Before release: physical Android ARM32/ARM64 installation and read/pack/save/reopen/unpack tests; publisher-keystore and independently reviewed release signature; explicit format-interoperability receipts if wanted.

Authorial adapter/client: rmr/LICENSE_RMR. Original BLAKE3: upstream author/license, unchanged. Individual-research-only license files apply exclusively where individually bound; not implicitly to upstream or new code.

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM.
No auto-merge, no Pages publishing, no release promotion by this change.

R3 = <F_ok: independent Android candidate, F_gap: remote/physical/format/signing evidence, F_next: inspect exact-head CI and obtain device receipts>.
