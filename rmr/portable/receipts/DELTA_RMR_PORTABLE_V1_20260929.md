# RMR Portable V1 — Delta Receipt

Date: 2026-09-29

```text
base = a5c5a82690356faeae1400bb65e6821766f9b1d9
branch = feature/rmr-portable-unified-v1
state = IMPLEMENTED_UNTESTED
```

## Materialized

- prospective individual-research source license;
- path/license provenance matrix;
- fixed256 C pure core with unrolled frame;
- batch4 unrolled API;
- direct BLAKE3 provider adapter without modifying upstream;
- no-libc memory shim for freestanding provider link;
- Rust `no_std` ABI;
- Java fixed-frame mirror with no import/I/O/JNI in kernel;
- host semantic/KAT tests;
- x86_64 / AArch64 / ARMv7 freestanding link matrix;
- every-file source manifest generator;
- audit/legal/order-form templates.

## Boundaries

```text
UPSTREAM_BLAKE3_REWRITTEN = false
UPSTREAM_LICENSE_CHANGED = false
OLD_RMR_GRANT_REVOKED = false
NEW_PORTABLE_LICENSE = PROSPECTIVE_ONLY
APK = NOT_IMPLEMENTED_THIS_DELTA
C_API = IMPLEMENTED_UNTESTED
RUST_NOSTD = IMPLEMENTED_UNTESTED
JAVA_FIXED256 = IMPLEMENTED_UNTESTED
CROSS_ARCH = PENDING_CI
BLAKE3_PROVIDER_KAT = PENDING_CI
```

## F_next

Run source-shape -> host KAT -> cross-link -> ELF audit -> manifest receipt.
