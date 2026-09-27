# RMR Crypto Freestanding 140 — START HERE

**State:** AUTHORIAL_FRAMEWORK / REGISTRY_COMPLETE / IMPLEMENTATION_PARTIAL  
**Date:** 2026-09-26  
**Authority:** RMR authorial framework and code only. Standard cryptographic algorithms remain attributed to their specifications/authors.

## Purpose

This directory is the freestanding research/implementation surface for the 140 cryptographic seed addresses carried by the RMR/Papers workflow.

It is designed to converge toward:

```text
no libc
no heap
no garbage collector
no filesystem in kernels
no clock in kernels
no dynamic loader
no provider dependency in kernels
caller-owned fixed buffers
fixed-width integer semantics
cross-architecture link proof
KAT before promotion
negative tests before claims
```

## Critical authorship boundary

```text
authorial RMR implementation != invention of standard primitive
formula/specification != RMR copyright claim
integration methodology != ownership of external standard
```

RMR may author:
- source organization;
- ABI;
- fixed-memory kernels;
- branchless/unrolled implementation choices;
- build/link pipeline;
- receipts;
- custody/provenance;
- test harnesses;
- orchestration methodology.

RMR does not silently claim authorship of AES, SHA, ChaCha, EdDSA, ML-KEM, Argon2 or other named primitives.

## The 140 addresses

Canonical source copy:
`registry/CRYPTO_140_SOURCE.tsv`

Normalized RMR state:
`registry/CRYPTO_140_FREESTANDING_STATE.tsv`

Exactly 140 seed IDs are preserved.

## Source-shape rule

For files under `kernel/` the V1 style target is:
- no `if (...)`;
- no `for (...)`;
- no heap;
- no libc;
- no recursive control flow;
- fixed-size data movement where practical.

This is a code-shape policy, **not a claim that every cryptographic primitive can be expressed using XOR alone**.

## XOR boundary

XOR is central to many primitives but is not sufficient for all of them.

Examples:
- SHA-2 requires modular addition, rotations/shifts and Boolean functions.
- ChaCha20 requires add-rotate-XOR.
- AES requires substitution and finite-field structure.
- elliptic-curve signatures/key exchange require field arithmetic.
- password KDFs require memory/iteration contracts.

Therefore:

```text
XOR_ONLY_FOR_ALL_140 = MATHEMATICALLY_UNSOUND
```

The framework permits only operations required by the selected standard and rejects gratuitous abstraction.

## Q8/Q16/Q32/Q42

Q profiles are available to orchestration/measurement layers, but **cryptographic primitive semantics remain bit-exact to their specifications**.

Do not replace a standard 32-bit word or field element with Q16/Q42 merely to fit a global style.

## Promotion ladder

```text
REGISTERED
-> SPEC_PINNED
-> AUTHORIAL_SOURCE
-> FREESTANDING_OBJECT
-> LINK_PROVED
-> KAT_PASS
-> NEGATIVE_PASS
-> CROSS_ARCH_PASS
-> REVIEWED
```

No step is inferred from the previous one.

## Navigation

- `AUTHORSHIP_AND_AUTHORITY.md`
- `CONTRACT.md`
- `STATE_MATRIX.md`
- `ROADMAP.md`
- `registry/`
- `families/`
- `kernel/`
- `build/`
- `audit/`
- `receipts/`
