<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Ed25519 — RMR algorithm contract

```yaml
algorithm_id: ed25519
class: signature
provider: OpenSSL-3-EVP_PKEY
status: IMPLEMENTED_PROVIDER
security_use: current
runtime_authority: rmr/crypto/runtime/registry.json
runtime_adapter: rmr/crypto/runtime/src/rmr_crypto_openssl.c
kat_surface: rmr/crypto/runtime/tests/rmr_crypto_runtime_selftest.c
claim_allowed: false
```

Digital signature with RFC 8032 KAT and negative verification.

This directory is a contract/provenance address; it does not duplicate the primitive.

`ALGORITHM != PROVIDER != ARCHITECTURE != EXECUTION != EVIDENCE != CLAIM`
