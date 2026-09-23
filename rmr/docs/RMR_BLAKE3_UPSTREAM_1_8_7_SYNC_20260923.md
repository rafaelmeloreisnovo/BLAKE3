# RMR BLAKE3 — upstream 1.8.7 synchronization

State: IMPLEMENTED_UNTESTED  
Claim allowed: false

## Exact refs

- upstream: `BLAKE3-team/BLAKE3@6aab490a26124663329dfd3961b8469f8fdb158b`
- fork parent: `141f8065e2c3845978cff7aabb6eeadebc155d30`
- branch: `sync/upstream-1.8.7-rmr-preserve-20260923`

## Synchronization boundary

The upstream delta since the shared base changes 39 paths. Thirty-five paths
had no fork-side source delta and are imported directly. Four paths are merged
semantically:

- `Cargo.toml`: upstream 1.8.7 metadata/dependencies/edition + RMR
  release/bench LTO and one-codegen-unit policy.
- `c/CMakeLists.txt`: upstream 1.8.7 version and clang-cl/MSVC SIMD flag
  routing + preserved RMR CMake 3.9 form, TBB pkg-config behavior and explicit
  C++ frontend routing.
- `c/blake3.h`: upstream 1.8.7 version + preserved `BLAKE3_RESTRICT`.
- `README.md`: RMR documentation preserved and exact upstream baseline added.

`build.rs` is imported from upstream 1.8.7 because no fork source delta
existed there since the shared base. This includes current upstream handling
that appends `-fno-lto` to explicit CFLAGS for C intrinsics when required to
avoid C-compiler/Rust-linker LTO mismatch.

## Gate

```text
SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM
IMPLEMENTED_UNTESTED != PASS
```

Required before performance promotion: Rust tests, C strict warnings/CTest,
installed consumer KAT, differential semantic modes, backend matrix,
common-harness official-vs-fork benchmark, cross-architecture compile, and
separate physical ARM receipts.
