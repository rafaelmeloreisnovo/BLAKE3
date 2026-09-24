<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Market algorithm expansion backlog

V2 promoted SHA-224, SHA-384, SHA3-512, SHAKE128, SHAKE256, BLAKE2s-256,
Ed448 and X448.

V3 promotes SHA-512/224, SHA-512/256, SHA3-224, SHA3-384 and SM3 into the
runtime registry with provider/API/KAT surfaces. SM3 is classified as
`standards_interoperability`, not as a universal recommendation.

Remaining candidates stay fail-closed:

- RIPEMD-160 only under an explicit compatibility/provider policy;
- HMAC-SHA384 and HMAC-SHA3 profiles if an application contract requires them;
- HKDF-SHA512 if a key schedule contract requires it;
- scrypt and Argon2id under a separate password-KDF dependency decision;
- post-quantum KEM/signature families only after provider/version/licensing
  boundaries are explicitly selected.

State for every unimplemented candidate: `TOKEN_VAZIO_NOT_IMPLEMENTED`.

Promotion still requires directory contract -> registry -> provider/API -> KAT ->
negative/error-path tests when applicable -> native CI -> architecture/provider
receipt -> documentation.
