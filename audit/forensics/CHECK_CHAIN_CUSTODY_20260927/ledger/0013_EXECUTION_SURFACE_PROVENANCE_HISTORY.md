# μ0013 — Execution-surface provenance history

- date: 2026-09-27
- parent_commit: 91abdfc82aa3eaf56cbc937fbf3cbeea431397c2
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: PATH_HISTORY_AND_RESPONSIBILITY_BOUNDARY
- claim_allowed: false

## Responsibility rule

Git history can establish recorded author/committer metadata for a path change. It does not by itself establish motive, operational control of a later run, or malicious intent.

`COMMIT_AUTHOR != LATER_RUNTIME_CONTROLLER`
`COMMITTER != INTENT`

## RMR shared resolver

Path: `rmr/upstream_validation/common.sh`

Observed path history in the fork:
- commit `137fda82234f56ef4e23d8cd0b0f6f66e40f5033`
- date: 2026-09-23T08:18:08Z
- author/committer metadata: Rafael mreis / rafaelmeloreisnovo
- subject: `test(rmr): add common.sh`

This path contains the default upstream repository/ref and the clone/fetch helper described in μ0012.

## RMR comparison orchestrator

Path: `rmr/tools/orchestrate_blake3_compare_v2.sh`

Observed path history:
- commit `36d19752b6e0dd6643db66a4ed84ea0c42112a0e`
- date: 2026-09-23T08:09:49Z
- author/committer metadata: Rafael mreis / rafaelmeloreisnovo
- subject: `chore(rmr): promote orchestrate_blake3_compare_v2.sh into full validation branch`

This establishes recorded provenance of the fork-side path, not the origin of every command/tool it invokes.

## Root build.rs

Path history returned 30 commits.

The current fork baseline includes:
- `774ba7ec9639a79132a6992e35f16035707f58d9`
- 2026-09-23T10:08:30Z
- Rafael mreis / rafaelmeloreisnovo
- `sync(rmr): align fork baseline with upstream BLAKE3 1.8.7`

Earlier history contains upstream contributions by multiple authors/committers, including Jack O'Connor, Ivan Boldyrev, Sporif, rsdy, Matthew Krupcale, Erik Johansson and others.

The native build-selection mechanism predates the fork synchronization and has multi-author upstream provenance.

## C Rust-bindings build.rs

Path history returned 14 commits.

Current fork baseline again includes `774ba7ec...` synchronization.

Earlier history includes upstream changes by Jack O'Connor, Sporif, rsdy, Matthew Krupcale, Erik Johansson and Joel Rosdahl, among others.

Notable upstream historical subjects include:
- initial C Rust bindings;
- integration of assembly implementations;
- cross-test support;
- TBB support;
- file traversal/rerun behavior.

## c/CMakeLists.txt

Path history returned 50 commits.

Fork-side entries include:
- `774ba7ec...` — sync to upstream BLAKE3 1.8.7;
- `8c6598dc...` — Reisolate upstream core from external RMR layer;
- `47ea07ed...` — ARM ABI markers/documentation;
- `a29d4d81...` — RMR dispatch flags/build selection.

Earlier history includes upstream contributions from Jack O'Connor, Henrik Gaßmann, silvanshade, SteveGremory, Keith Winstein, Rui Ueyama and others.

TBB/CMake/SIMD/package configuration therefore has both upstream historical provenance and fork-side synchronization/customization history.

## Forensic classification

- recorded path author/committer: OBSERVED
- fork synchronization points: OBSERVED
- upstream multi-author ancestry: OBSERVED
- later runtime controller inferred from author: PROHIBITED_INFERENCE
- deliberate concealment inferred from path history: NOT_ESTABLISHED
- unauthorized modification: NOT_PROVEN

## R3

- F_ok: provenance separated from runtime causality.
- F_gap: exact CMake/package-resolution behavior still requires state-by-state expansion.
- F_next: map CMake defaults, optional TBB resolution, workflow overrides, and generated build edges.
