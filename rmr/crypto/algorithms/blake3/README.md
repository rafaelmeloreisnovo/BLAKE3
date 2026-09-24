<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# BLAKE3 — RMR algorithm contract

```yaml
algorithm_id: blake3
class: hash
provider: BLAKE3-upstream
status: IMPLEMENTED
security_use: current
runtime_authority: rmr/crypto/runtime/registry.json
runtime_adapter: rmr/crypto/runtime/src/rmr_crypto_blake3.c
kat_surface: rmr/crypto/runtime/tests/rmr_crypto_runtime_selftest.c
claim_allowed: false
```

Normal RMR execution links the current repository checkout `c/`; the official team repository is fetched only by comparison/validation harnesses.

This directory is a contract/provenance address; it does not duplicate the primitive.

`ALGORITHM != PROVIDER != ARCHITECTURE != EXECUTION != EVIDENCE != CLAIM`
