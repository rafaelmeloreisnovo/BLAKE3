<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Crypto Runtime Matrix V1

State: IMPLEMENTED_PENDING_CI  
Claim allowed: false

## Boundary

```text
ALGORITHM != PROVIDER != ARCHITECTURE
BLAKE3_BACKEND_PASS != OPENSSL_PROVIDER_PASS
COMPILE_CONTRACT != LINK_PROVIDER != EXECUTION != EVIDENCE != CLAIM
```

This directory operationalizes cryptographic primitives without importing or
renaming the BLAKE3 algorithm. BLAKE3 is called through its existing upstream
C API. The additional primitives are delegated to OpenSSL 3 provider APIs.

## Algorithms

- BLAKE3 — upstream adapter;
- MD5 — compatibility only;
- SHA-1 — compatibility only;
- SHA-256;
- SHA-512;
- HMAC-SHA256;
- HKDF-SHA256;
- Ed25519;
- ChaCha20-Poly1305;
- AES-256-GCM;
- SHA3-256;
- BLAKE2b-512;
- HMAC-SHA512;
- X25519;
- PBKDF2-HMAC-SHA256;
- SHA-224;
- SHA-384;
- SHA3-512;
- SHAKE128;
- SHAKE256;
- BLAKE2s-256;
- Ed448;
- X448;
- SHA-512/224;
- SHA-512/256;
- SHA3-224;
- SHA3-384;
- SM3.

MD5 and SHA-1 are present for interoperability/regression work. Their presence
does not promote them for new security-sensitive designs.

## Three initial additional primitives

The three added beyond the requested hash/MAC/signature set are:

1. HKDF-SHA256 — key derivation;
2. ChaCha20-Poly1305 — AEAD;
3. AES-256-GCM — AEAD.

## Five additional primitives — second expansion

1. SHA3-256 — modern SHA-3 hash;
2. BLAKE2b-512 — independent high-speed hash family;
3. HMAC-SHA512 — wider HMAC profile;
4. X25519 — key agreement;
5. PBKDF2-HMAC-SHA256 — password-based derivation/interoperability.

These five are provider-backed and must pass their own KAT/provider gate. They
do not inherit a BLAKE3 architecture result.

## Eight additional primitives — third expansion

1. SHA-224 and SHA-384 — SHA-2 variants;
2. SHA3-512 — wider SHA-3 digest;
3. SHAKE128 and SHAKE256 — extendable-output functions with explicit output length;
4. BLAKE2s-256 — BLAKE2s digest;
5. Ed448 — EdDSA signature profile;
6. X448 — Curve448 key agreement.

The V2 Cartesian registry contains 23 algorithms x 9 architecture profiles =
207 registered cells.

## Five additional primitives — fourth expansion

1. SHA-512/224 and SHA-512/256 — SHA-512-derived SHA-2 variants with distinct IVs;
2. SHA3-224 and SHA3-384 — remaining fixed-output SHA-3 sizes;
3. SM3 — 256-bit hash retained as a standards/interoperability profile.

The V3 target is 28 algorithms x 9 architecture profiles = 252 registered
cells. Only the native profile can become KAT_PASS_NATIVE in a single host run;
provider execution on the other profiles remains TOKEN_VAZIO until separately
observed. SM3 presence is not a global algorithm recommendation.

## BLAKE3 architecture map

The canonical matrix is `registry.json`.

Optimized BLAKE3 lanes observed in this repository include:

- x86/x86_64: SSE2, SSE4.1, AVX2, AVX-512 F+VL;
- ARMv7: NEON when explicitly enabled/supported;
- 32-bit ARMv8: NEON route;
- AArch64/ARM64: NEON on the little-endian optimized route;
- wasm32: WASM SIMD when the feature is enabled;
- portable scalar: universal fallback.

RISC-V and PowerPC are registered only as portable-fallback mappings. They are
not mislabeled as dedicated optimized BLAKE3 SIMD backends.

The x86_64 source tree additionally contains Unix, Windows GNU and Windows MSVC
assembly variants.

## Contract cross-matrix

`build/build_contract_matrix.sh` compiles the provider-neutral RMR contract
for:

```text
x86_64
x86-32
ARMv7
AArch64
WASM32
RISC-V64
PPC64LE
```

That proves header/ABI compilation only. It does not prove an OpenSSL provider
exists for every cross target.

## Provider rule

The OpenSSL runtime is built only when OpenSSL >= 3.0 Crypto is available.
Architecture-specific acceleration for MD5/SHA/HMAC/Ed25519/AEAD belongs to
the provider and is not inferred from the BLAKE3 SIMD backend.

## KATs

`tests/rmr_crypto_runtime_selftest.c` checks:

- BLAKE3 "abc";
- MD5/SHA-1/SHA-256/SHA-512 "abc";
- HMAC-SHA256;
- RFC 5869 HKDF-SHA256 test case 1;
- RFC 8032 Ed25519 test 1, including negative verification;
- AES-256-GCM fixed known-answer vector;
- RFC 8439 ChaCha20-Poly1305 AEAD vector;
- SHA3-256 "abc";
- RFC 7693 BLAKE2b-512 "abc";
- RFC 4231 HMAC-SHA512;
- RFC 7748 X25519 public/shared-secret vectors;
- PBKDF2-HMAC-SHA256 fixed interoperability vector;
- SHA-224/SHA-384/SHA3-512/BLAKE2s-256 "abc" KATs;
- SHAKE128 (32-byte output) and SHAKE256 (64-byte output) "abc" KATs;
- RFC 8032 Ed448 blank-message test vector plus negative verification;
- RFC 7748 X448 public-key and shared-secret vectors;
- SHA-512/224, SHA-512/256, SHA3-224 and SHA3-384 "abc" KATs;
- SM3 "abc" standards/interoperability KAT.

## Validation

```sh
python3 rmr/crypto/runtime/tools/validate_runtime_registry.py
sh rmr/crypto/runtime/build/build_contract_matrix.sh

cmake -S rmr -B build/rmr-crypto -G Ninja \
  -DRMR_BUILD_TESTS=ON \
  -DRMR_BUILD_CRYPTO_RUNTIME=ON
cmake --build build/rmr-crypto --target rmr-crypto-runtime-selftest
ctest --test-dir build/rmr-crypto -R rmr_crypto_runtime_selftest --output-on-failure
```

Provider execution on a specific non-native architecture remains
`TOKEN_VAZIO_PROVIDER_TOOLCHAIN` until that architecture has an observable
toolchain/provider receipt.
