# RMR ACTION PIN MANIFEST V1 — 2026-09-28

- repository: `rafaelmeloreisnovo/BLAKE3`
- base_commit: `4358dfd56e7b26d8e5e5363820ca8edb493363ed`
- branch: `hardening/rmr-action-pins-v1-20260928`
- scope: `.github/workflows/rmr-*.yml`
- upstream_ci_yml_modified: `false`
- claim_allowed: `false`

## Purpose

Replace mutable major-tag GitHub Action references in fork-local RMR workflows with full Git commit SHAs that were observed in real successful workflow executions.

This delta intentionally does **not** modify the upstream-identical `.github/workflows/ci.yml`.

## Runtime-proven action objects

### actions/checkout v6

Pinned object:

`d23441a48e516b6c34aea4fa41551a30e30af803`

Evidence:
- BLAKE3 post-merge full-validation run: `36382699560`
- aggregate job: `108802132953`
- log line observed:
  `Download action repository 'actions/checkout@v6' (SHA:d23441a48e516b6c34aea4fa41551a30e30af803)`

Earlier frozen Rust/Cargo evidence independently observed the same action SHA.

### actions/checkout v4

Pinned object:

`11d5960a326750d5838078e36cf38b85af677262`

Evidence:
- BLAKE3 post-merge CF140 run: `36382699509`
- job: `108801553712`
- log line observed:
  `Download action repository 'actions/checkout@v4' (SHA:11d5960a326750d5838078e36cf38b85af677262)`

The same log emitted a Node.js 20 deprecation warning and stated the runner forced the action to Node.js 24.

Pinning the Git object does not erase that runtime compatibility/deprecation boundary.

### actions/upload-artifact v4

Pinned object:

`ea165f8d65b6e75b540449e92b4886f43607fa02`

Evidence:
- BLAKE3 post-merge full-validation run: `36382699560`
- aggregate job: `108802132953`
- runtime log resolved `actions/upload-artifact@v4` to that SHA.

Earlier frozen Rust/Cargo evidence independently observed the same SHA.

### actions/download-artifact v4

Pinned object:

`d3f86a106a0bac45b974a628896c90dbdf5c8093`

Evidence:
- BLAKE3 post-merge full-validation run: `36382699560`
- aggregate job: `108802132953`
- runtime log resolved `actions/download-artifact@v4` to that SHA.

The log also recorded the current Node.js runtime deprecation/forced-runtime warning for artifact actions.

### actions/setup-python v6

Pinned object:

`ece7cb06caefa5fff74198d8649806c4678c61a1`

Evidence:
- historical ZIP custody run: `36286488972`
- job: `108528130492`
- runtime log:
  `Download action repository 'actions/setup-python@v6' (SHA:ece7cb06caefa5fff74198d8649806c4678c61a1)`

## Static post-change gate

Inspected RMR workflow set:

1. `rmr-blake3-repro.yml`
2. `rmr-blake3-upstream-compare-v2.yml`
3. `rmr-crypto-freestanding140-v1.yml`
4. `rmr-crypto-runtime.yml`
5. `rmr-full-validation.yml`
6. `rmr-hardware-build-topology.yml`
7. `rmr-hash-boundary-v1.yml`
8. `rmr-pure-core-boundary-v1.yml`
9. `rmr-simperf.yml`
10. `rmr-standalone-v1.yml`
11. `rmr-upstream-comprehensive-v3.yml`
12. `rmr-zip-custody.yml`

Result:

`RMR_WORKFLOWS_WITH_MAJOR_TAG_USES=0`

All observed `uses:` references in those 12 files now use full commit SHAs.

Readable comments preserve the corresponding major version, e.g.:

`actions/checkout@<40-hex> # v6`

## What this proves

After this source delta, a future RMR workflow run no longer asks GitHub to resolve those five action families through mutable major-version refs.

It instead requests the exact recorded Git action objects.

## What this does NOT prove

This change does not make the complete CI execution hermetic.

Remaining mutable/external boundaries include, depending on workflow:

- GitHub-hosted runner image contents;
- Rust channels/toolchains outside these RMR action pins;
- OS package-manager resolution;
- remote installers;
- Cargo dependency resolution where no frozen closure governs;
- external Git/network inputs already separately inventoried;
- Node runtime behavior supplied by GitHub Actions runner.

## Upstream boundary

`.github/workflows/ci.yml` remains outside this hardening delta.

Reason:
that file is maintained as the upstream BLAKE3-team compatibility baseline and was previously proven byte-identical at the audited state. Fork-local custody hardening must not silently rewrite upstream compatibility semantics.

## State before runtime validation

- action SHA provenance: `PASS_RUNTIME_OBSERVED`
- RMR source pinning: `IMPLEMENTED`
- static no-major-tag gate: `PASS`
- post-change workflow execution: `NOT_RUN`
- claim_allowed: `false`

## R3

- F_ok: every GitHub Action reference in the inspected RMR workflow set is pinned to a runtime-observed full SHA.
- F_gap: post-change PR workflow validation has not yet executed; other non-action mutable supply-chain edges remain.
- F_next: open a review PR, execute affected RMR workflows, enumerate all check-runs completely, and promote only after terminal evidence.
