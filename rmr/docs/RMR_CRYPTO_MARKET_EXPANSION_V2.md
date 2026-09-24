<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Crypto Market Expansion V2

This delta promotes eight provider-backed primitives from TOKEN_VAZIO backlog to
implemented runtime surfaces:

SHA-224, SHA-384, SHA3-512, SHAKE128, SHAKE256, BLAKE2s-256, Ed448 and X448.

Evidence design:
- SHA-2/SHA-3/SHAKE/BLAKE2s use OpenSSL EVP and deterministic KATs;
- Ed448 uses the RFC 8032 blank-message vector and negative verification;
- X448 uses RFC 7748 Alice/Bob public-key and shared-secret vectors;
- enum additions are appended, preserving existing numeric enum values;
- the CI matrix is dynamic: algorithms x architectures, avoiding a stale 135-cell constant.

Provider execution on non-native architecture profiles remains
`TOKEN_VAZIO_PROVIDER_TOOLCHAIN` until directly observed.

`SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM`
