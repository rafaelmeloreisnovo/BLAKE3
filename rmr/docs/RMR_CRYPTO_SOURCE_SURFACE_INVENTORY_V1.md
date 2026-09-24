<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Crypto Source Surface Inventory V1

| Path | Role | Authority |
|---|---|---|
| `rmr/core/hash_blake3.c` | PAI BLAKE3 adapter | fork checkout `c/` |
| `rmr/crypto/runtime/src/rmr_crypto_blake3.c` | runtime BLAKE3 adapter | fork checkout `c/` |
| `rmr/core/hash_sha256.c` | hosted SHA-256 implementation | RMR source |
| `rmr/freestanding_custody16/src/rmr_fc16_sha256.c` | freestanding SHA-256 | RMR source |
| `rmr/crypto/runtime/src/rmr_crypto_openssl.c` | MD5/SHA/SHA3/SHAKE/BLAKE2/HMAC/HKDF/Ed25519/Ed448/X25519/X448/AEAD/PBKDF2 adapter | OpenSSL 3 |
| `rmr/freestanding_custody16/src/rmr_fc16_crc32c.c` | CRC32C + CRC-32 | checksum, non-cryptographic |

`SOURCE_SHA256.txt`, `*.sha256`, `RMR_ARTIFACTS_SHA256.txt` and generated
`SHA256SUMS` are evidence/fingerprints, not implementations.

```text
normal RMR BLAKE3 -> current fork checkout /c
official comparison -> https://github.com/BLAKE3-team/BLAKE3.git @ OFFICIAL_REF
OpenSSL algorithms -> provider on executing host/target
```

`SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM`
