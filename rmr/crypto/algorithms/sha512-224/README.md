<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# SHA-512/224 — RMR algorithm contract

```yaml
algorithm_id: sha512-224
class: hash
provider: OpenSSL-3-EVP
status: IMPLEMENTED_PROVIDER
security_use: current
runtime_authority: rmr/crypto/runtime/registry.json
runtime_adapter: rmr/crypto/runtime/src/rmr_crypto_openssl.c
kat_surface: rmr/crypto/runtime/tests/rmr_crypto_runtime_selftest.c
claim_allowed: false
```

SHA-2 digest using the SHA-512 family with a distinct initial value; KAT uses digest of abc.

`ALGORITHM != PROVIDER != ARCHITECTURE != EXECUTION != EVIDENCE != CLAIM`
