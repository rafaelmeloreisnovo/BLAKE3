# Crypto Freestanding 140 — Roadmap V1

## F0 — registry/authority
State: IMPLEMENTED_THIS_DELTA

- preserve all 140 addresses;
- normalize taxonomy;
- mark legacy/ambiguous entries;
- distinguish existing provider/upstream references from new code.

## F1 — substrate
State: SOURCE_IMPLEMENTED / CI_PENDING

- compiler-native types;
- fixed byte/word XOR;
- rotate/shift helpers;
- branchless select/masks;
- fixed-size equality;
- endian load/store;
- no-if/no-for source gate;
- cross-link x86_64/AArch64/ARMv7.

## F2 — hash family
State: STARTED — H01 compression core source implemented; CI pending

Priority:
1. SHA-256 — reuse/port the existing authorial memory core after KAT equivalence.
2. SHA-512 family.
3. SHA-3/SHAKE.
4. BLAKE2 family.
5. SM3 interoperability.
6. legacy hashes only in quarantined compatibility modules.

BLAKE3 remains upstream-authority unless a separately named experimental RMR primitive is defined. Do not relabel a reimplementation as upstream BLAKE3 without conformance.

## F3 — MAC/KDF
State: PLANNED

HMAC/HKDF first, built from promoted hash cores.

Password KDFs require explicit fixed-memory policy; Argon2/scrypt cannot be made “no memory” without ceasing to be those algorithms.

## F4 — symmetric/AEAD
State: PLANNED

- ChaCha20/Poly1305;
- AES family;
- GCM/CTR/CCM/SIV etc only after base primitive contracts.

## F5 — public key
State: PLANNED

Curve/signature/KEX families require big-integer/field modules with exact constant-time review.

## F6 — post-quantum
State: PLANNED

ML-KEM / ML-DSA / SLH-DSA only after exact standard/version, parameter sets and large fixed workspace contracts are pinned.

## F7 — compatibility quarantine
State: PLANNED

MD5, SHA-1, DES, RC4, RSA-1024, DH-1024 and similar entries must stay visibly non-recommended for new security use.

## F8 — evidence
State: CONTINUOUS

Every promotion emits:
- source hash;
- toolchain;
- target;
- object/ELF hash;
- symbol audit;
- KAT receipt;
- negative receipt;
- cross-arch comparison;
- rollback pointer.
