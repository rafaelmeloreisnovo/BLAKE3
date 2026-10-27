# RMR prospective release-evidence — v0.2.0-rc.1 (DRAFT/HOLD)

New, isolated C11 fixed 64-byte **source-release evidence frame**. It carries a caller-supplied SHA-256 digest of a proposed asset, an opaque 16-byte reference and CRC32C. This is a binary versioned *metadata/transport integrity* utility, NOT a license enforcer, not a secure signature verifier, not a source-code hash engine, not DRM and not proof of authorship/ownership.

The frame always marks `DRAFT`: status=1. Values representing signed status, unknown version, reserved bits, zero digest and CRC mismatch are rejected; checking status is not verification that a legal agreement exists. No path to silently turn this frame into a legal authorization.

## Build

`sh rmr/prospective_release_v020/build/check.sh`

Requirements: C11 Clang or GCC, shell, standard command-line build tools. No internet or source dependencies. Core: no libc/CRT/heap/syscalls/threads/providers, and fixed caller-owned buffers; hosted test harness uses compiler runtime. `nm -u` is a source-object closure check, not evidence of ARM device execution. Build script uses a self-created scratch directory; does not honor or erase any custom `OUT` path.

## Reversibility

Additive directory only. No upstream file, existing license, previously published RMR file, build graph, default action, ABI or Android path is changed. Rollback: close unmerged draft PR or drop this directory from the release-candidate branch. No tags, packages, merges or releases until legal and exact-head CI/device checks are independently reviewed.
