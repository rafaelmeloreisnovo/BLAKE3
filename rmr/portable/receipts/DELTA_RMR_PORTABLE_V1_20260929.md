# RMR Portable V1 — Delta Receipt

Date: 2026-09-29

```text
base = a5c5a82690356faeae1400bb65e6821766f9b1d9
binary_fix_merge = 5e4c984074966c9ba6fbb4385d165ab52112d744
binary_validation_run = 36565932252 / RMR Portable V1 #20
state = PASS_CI
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
- fork-vs-upstream blob comparison registry;
- audit/legal/order-form templates.

## Binary validation — PASS

```text
external_js_actions = 0
source_shape = PASS
host_C = PASS
BLAKE3_abc_KAT = PASS
Java_fixed256 = PASS
Rust_no_std = PASS

core_x86_64_sha256 = 60073aba4d435bba078c7088904c663fa834525f640a0de2fcae1cafab5ab581
core_aarch64_sha256 = 33897588e43dccffecc669c6199c2c2f8534f4e293fa09f32962af9b3e12ea76
core_armv7_sha256 = bdb559ba7acdcc4f1c90ea93bec7549dddf9b34666c5b4625bb141358106117f

provider_x86_64_sha256 = 98aa0fdf704a41ece71dff4d871f852984913add46b111cf6b0f944edf10f65b
provider_aarch64_sha256 = 0705ca07c00c58eae5d696cd54a9a07e865c2bdb1b51e4eb42fc9a96cf46df9e
provider_armv7_sha256 = 8869c14a4ade17b42e085e60daaccb10f9f0a24538034cd672cd6a36a3c8a7b3

PT_INTERP = absent on all six audited ELF
DT_NEEDED = absent on all six audited ELF
unexpected_UND = 0 on all six audited ELF
cross_matrix = PASS
```

## Provenance refinement

The previous manifest covered 829 files and marked 50 paths as
`MIXED_OR_UNRESOLVED`.

Those 50 paths are now bound to an objective comparison registry against:

```text
upstream = BLAKE3-team/BLAKE3
upstream_head = 6aab490a26124663329dfd3961b8469f8fdb158b
comparison = path + Git blob SHA
```

The refined classes distinguish:

```text
UPSTREAM_BLAKE3_EXACT
UPSTREAM_PATH_DIVERGENT_BLOB
FORK_RAFAELIA_MATERIAL
FORK_TOOLING_ADDITION
FORK_MEDIA_ADDITION
MIXED_OR_UNRESOLVED
```

Origin classification and license certainty remain separate. A fork-added file
may have known structural origin while its file-level license remains
`TOKEN_VAZIO`.

## Dynamic final receipt

The CI generates the source manifest first, then creates a dynamic final
receipt from its SHA-256 and counters. This avoids a circular self-hash in
which a committed receipt would contain the hash of a manifest that itself
contains the receipt.

```text
SOURCE_MANIFEST -> SHA256 -> DYNAMIC_FINAL_RECEIPT
```

The dynamic receipt is evidence from the exact CI run and is recorded in the
workflow log / pull-request evidence.

## Boundaries

```text
UPSTREAM_BLAKE3_REWRITTEN = false
UPSTREAM_LICENSE_CHANGED = false
OLD_RMR_GRANT_REVOKED = false
NEW_PORTABLE_LICENSE = PROSPECTIVE_ONLY
APK = NOT_IMPLEMENTED_THIS_DELTA
C_API = PASS
RUST_NOSTD = PASS
JAVA_FIXED256 = PASS
CROSS_ARCH = PASS
BLAKE3_PROVIDER_KAT = PASS
PROVENANCE_FINAL = PASS_CI
```


## Final provenance promotion

The provenance-only gate completed successfully before this static promotion:

```text
run = RMR Portable V1 #26 / 36566818526
source_manifest_files = 830
upstream_exact_review_paths = 13
upstream_divergent_review_paths = 7
fork_addition_review_paths = 30
unresolved_origin = 0
license_token_vazio = 30
```

The exact manifest and dynamic-receipt SHA-256 values remain CI evidence rather
than being copied into this tracked file, preventing a self-referential
manifest/receipt hash cycle.

```text
origin_completeness = PASS
license_completeness = PARTIAL_TOKEN_VAZIO_30
```
