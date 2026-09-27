<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under ../LICENSE_RMR.
-->

# RMR Standalone Foundation Contract V1

## Scope

The foundation supplies only low-level primitives that can be linked into freestanding RMR modules.

## Invariants

```text
heap_calls = 0
libc_calls = 0
os_calls = 0
filesystem_calls = 0
clock_calls = 0
network_calls = 0
dynamic_loader_calls = 0
```

## Memory

`rmr_sa_zero` and `rmr_sa_copy` operate only on caller-provided memory.

No bounds can be discovered magically. Capacity/length remain caller contracts.

## Equality

`rmr_sa_equal` scans all requested bytes and does not early-return on byte mismatch.

V1 does **not** claim constant-time execution across compiler, ISA, cache or microarchitecture.

## Fold

`rmr_sa_xor_fold32` is a deterministic non-cryptographic fixture/probe primitive.

It is forbidden as a replacement for BLAKE3, SHA-256, HMAC or digital signature.

## Fixed point

Supported fractional widths:

```text
Q8  -> 32-bit storage profile
Q16 -> 32-bit storage profile
Q32 -> 64-bit storage profile
Q42 -> 64-bit storage profile
```

These are representation profiles only. Arithmetic policy (rounding, saturation, multiplication widening and domain-specific error bounds) must be specified by each consuming module.

## Promotion

```text
SOURCE
-> HOST SELFTEST
-> FREESTANDING LINK
-> ELF AUDIT
-> CROSS-ARCH RECEIPT
-> CONSUMER INTEGRATION
```

No performance claim follows from a smaller binary alone.
