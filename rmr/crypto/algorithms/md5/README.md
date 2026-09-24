<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# MD5 — RMR algorithm contract

```yaml
algorithm_id: md5
class: hash
provider: OpenSSL-3-EVP
status: IMPLEMENTED_PROVIDER
security_use: compatibility_only
runtime_authority: rmr/crypto/runtime/registry.json
runtime_adapter: rmr/crypto/runtime/src/rmr_crypto_openssl.c
kat_surface: rmr/crypto/runtime/tests/rmr_crypto_runtime_selftest.c
claim_allowed: false
```

Compatibility/regression only; not for new collision-resistant security designs.

This directory is a contract/provenance address; it does not duplicate the primitive.

`ALGORITHM != PROVIDER != ARCHITECTURE != EXECUTION != EVIDENCE != CLAIM`
