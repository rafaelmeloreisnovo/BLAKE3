<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Crypto Algorithm Directory V1

Stable directory addresses for the 15 algorithms already executable through
`rmr/crypto/runtime/registry.json`.

| ID | Class | Provider | Security use |
|---|---|---|---|
| `blake3` | hash | BLAKE3-upstream | current |
| `md5` | hash | OpenSSL-3-EVP | compatibility_only |
| `sha1` | hash | OpenSSL-3-EVP | compatibility_only |
| `sha256` | hash | OpenSSL-3-EVP | current |
| `sha512` | hash | OpenSSL-3-EVP | current |
| `sha3-256` | hash | OpenSSL-3-EVP | current |
| `blake2b-512` | hash | OpenSSL-3-EVP | current |
| `hmac-sha256` | mac | OpenSSL-3-EVP_MAC | current |
| `hmac-sha512` | mac | OpenSSL-3-EVP_MAC | current |
| `hkdf-sha256` | kdf | OpenSSL-3-EVP_KDF | current |
| `ed25519` | signature | OpenSSL-3-EVP_PKEY | current |
| `chacha20-poly1305` | aead | OpenSSL-3-EVP_CIPHER | current |
| `aes-256-gcm` | aead | OpenSSL-3-EVP_CIPHER | current |
| `x25519` | key_agreement | OpenSSL-3-EVP_PKEY | current |
| `pbkdf2-hmac-sha256` | password_kdf | OpenSSL-3-EVP | current |

MD5 and SHA-1 remain `compatibility_only`.

SHA-256 has three distinct RMR surfaces:
- `rmr/core/hash_sha256.c` — hosted/internal implementation;
- `rmr/freestanding_custody16/src/rmr_fc16_sha256.c` — freestanding implementation;
- `rmr/crypto/runtime/src/rmr_crypto_openssl.c` — OpenSSL-provider runtime surface.

CRC32C/CRC-32 in `rmr_fc16_crc32c.c` are checksums, not cryptographic hashes.

Validation:
```sh
python3 rmr/crypto/runtime/tools/validate_runtime_registry.py
python3 rmr/crypto/runtime/tools/validate_algorithm_layout.py
python3 rmr/crypto/runtime/tools/validate_source_authority.py
```
