# RMR authorial-core extraction — V1 · 2026-10-29
Status: DRAFT / HOLD / docs-only phase 0. This does **not** declare verified legal ownership, completed refactor, build success, freestanding proof, or novelty.
Base source: `rafaelmeloreisnovo/BLAKE3` master, latest examined commit `718a8f8e200c42aecfdf5ffedcab6d130ed3dcfb`.
Contract anchors: [root AGENTS](../../AGENTS.md), [rmr AGENTS](../AGENTS.md), [RMR START HERE](00_START_HERE_RMR.md), [binary contract](RMR_BUILD_AND_BINARY_CONTRACT_V1.md), [RMR provenance](../PROVENIENCE.md).
Counterpart isolation: branch `audit/rmr-external-provenance-20261029-hold`; do **not** assume code/assets shared across branches until a controlled merge.

## Objective
Establish a **candidate project-developed** pure deterministic RMR core under `rmr/` without moving or rewriting historical sources. Extraction targets must pass file-level source/rights review before their implementation is treated as original. All upstream BLAKE3 implementation, algorithm and tests remain with their original rights and **untouched**.

## Sliced inventory and extraction sequence
| Candidate path | Observed contract status | Phase-1 action | Boundary to retain outside pure core |
| --- | --- | --- | --- |
| `rmr/pai42/src/` | implemented fixed-Q16, according to START HERE | map pure input/output, overflow, saturation, endian behavior, run KAT | Python/JVM/host adapters |
| `rmr/freestanding_custody16/` | freestanding core with hosted filesystem bridge | pin bytes and extract deterministic memory-only verification interface | host filesystem and clocks |
| `rmr/fixed256/asm/` | kernel ASM, not cryptographic hash | preserve exact ISA and XOR semantics; build isolated regression fixture | OS entry, SIMD policy |
| `rmr/portable/` | prospective project-developed surface, selective licensing | audit per-file licenses and provider calls, no mass edit | BLAKE3 provider, Rust/Java/Android bridges |
| `rmr/core/` & `rmr/ui/` | PAI hosted, filesystem/time/UI mixed | inventory pure math & fixed-size logic before any extraction | POSIX/libc/time/terminal |
| `rmr/crypto/` | mix of RMR tooling and OpenSSL-backed providers | classify each source path and expected digest | OpenSSL, BLAKE3, platform providers |
| `ZIPRAF_CORE` / `ZIPRAF_OMEGA_FULL` | independent repos, not paths in BLAKE3 | link as *external producer references only* until versioned API contract exists | codec implementations, licenses, historical files |
| `templo-vivo-arcs` | independent repository and historical source | link provenance by SHAs, not import archive code | Flutter, Android SDK, uploaded ZIP |

## Gate chain (required for every extraction)
1. `SOURCE`: repository, exact commit, file path, blob SHA, license, author evidence; missing rights = `TOKEN_VAZIO_RIGHTS`.
2. `INTERFACE`: exact byte layouts, endianness, null-vs-empty-vs-space, integer width and overflow contract.
3. `ISOLATION`: new **additive** source path, no changes to the pinned original; external provider only via explicit adapter.
4. `BUILD`: compiler, flags, ABI, ISA, link flags, ELF symbol/section report and artifact SHA-256.
5. `TEST`: deterministic KAT on original and candidate, boundary-value tests, negative tests; no hidden `|| true`.
6. `PERFORMANCE` (only if claimed): equivalent target, input, toolchain and repeated measurements.
7. `RIGHTS`: independent file-level license review before applying any new project-authorial license.
8. `PROMOTION`: draft PR, human review, no auto-merge, rollback by reverting only the new additive commit.

## Initial fixtures from 2025 conversation export
- A 2025-03-29 conversation titled `Criar APK Android` was located in the private NOVOexport `conversations-004.json` and predates the 2025-03-29 creation time reported for the Templo Vivo GitHub repository. The conversation includes ARCΣ/Flutter/APK-related material, but no private prompts or credentials are copied here.
- Templo Vivo first source commit `3ef3c20989d2ac2f9638983a89863587f44a39ad` dated 2025-03-31; ZIP upload commit `83d01cf7666495024da176c5bfe15cb3ff3a475d`; later disconnected root `03fdde9f1b54d4b037f4afa5df16c53075d8f06a` dated 2025-04-03. Temporal association is **not** code inheritance, legal authorship or build proof.
- Distinct `∅`, `""`, four spaces, `NULL`, `0`, byte `0x00` must remain separate in future typed-input gates.

## Runtime and security STOP conditions
- No third-party code duplication or license transfer.
- No automatic GitHub sync or public export of private raw conversation messages.
- No credential values copied from old scripts.
- No production source move or regex mass refactor without exact original blobs and relevant full test receipt.
- No claim `FREESTANDING_PROVED` without build binary gates and an explicit platform boundary.

## Baseline and pending work
`PASS` = source AGENTS and existing architecture consulted; branch created.
`NOT_RUN` = compiler, KAT, WARC/CDX crawl, Android runtime, repository-wide token parsing, copyright determination.
`TOKEN_VAZIO` = per-path author proof, independent 2012 ZIPRAF original, March 2005 Templo artifact, deploy bridge between IIS domain archives and GitHub repos, provider bot autonomy.
`F_ok` = isolated extraction branch and no-touch source preservation contract.
`F_gap` = production refactor is **not yet implemented**; major gates unresolved.
`F_next` = select the first candidate file from the table, pin Git blob, extract additive pure core and tests, compare outputs, submit draft PR.
