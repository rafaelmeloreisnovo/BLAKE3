<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# SM3 — RMR algorithm contract

```yaml
algorithm_id: sm3
class: hash
provider: OpenSSL-3-EVP
status: IMPLEMENTED_PROVIDER
security_use: standards_interoperability
runtime_authority: rmr/crypto/runtime/registry.json
runtime_adapter: rmr/crypto/runtime/src/rmr_crypto_openssl.c
kat_surface: rmr/crypto/runtime/tests/rmr_crypto_runtime_selftest.c
claim_allowed: false
```

256-bit SM3 digest exposed for standards/interoperability. Registration is not a global recommendation; KAT uses digest of abc.

`ALGORITHM != PROVIDER != ARCHITECTURE != EXECUTION != EVIDENCE != CLAIM`
