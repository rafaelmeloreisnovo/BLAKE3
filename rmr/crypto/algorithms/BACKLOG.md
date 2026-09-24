<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Market algorithm expansion backlog

The V2 promotion moved SHA-224, SHA-384, SHA3-512, SHAKE128, SHAKE256,
BLAKE2s-256, Ed448 and X448 into the runtime registry with code/KAT surfaces.

Remaining candidates stay fail-closed:

- SHA-512/224 and SHA-512/256;
- SHA3-224 and SHA3-384;
- SM3 where interoperability requires it;
- RIPEMD-160 only under an explicit compatibility/provider policy;
- scrypt and Argon2id under a separate password-KDF dependency decision.

State for every unimplemented candidate: `TOKEN_VAZIO_NOT_IMPLEMENTED`.

Promotion still requires directory contract -> registry -> provider/API -> KAT ->
negative/error-path tests when applicable -> native CI -> architecture/provider
receipt -> documentation.
