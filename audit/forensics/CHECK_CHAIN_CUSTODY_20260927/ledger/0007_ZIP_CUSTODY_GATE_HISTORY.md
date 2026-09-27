# μ0007 — ZIP custody gate history and late header failure

- date: 2026-09-27
- clock: TOKEN_VAZIO
- parent_commit: 4de3fb09d8e2a4c8aed42a55e969d7e1c2842746
- kind: HISTORICAL_DELTA_PLUS_EXECUTION
- claim_allowed: false

## History

### Initial gate

Commit `430ad910d13ff2f46740a0e30fecabe7b42c112b`
- author/committer: `rafaelmeloreisnovo`
- created `.github/workflows/rmr-zip-custody.yml`.
- included ZIP profile validation, adversarial unit tests, and a final RMR authorship-header verification step.

### Dependency reduction

Commit `2c1caf90b940f1b408851e0bc845cf54a70e8072`
- author/committer: `rafaelmeloreisnovo`
- removed pip-cache settings from Python setup.
- commit subject: `ci(rmr): keep ZIP custody gate dependency-free`.

### Forensic-time expansion

Commit `580132ce38edecf3b9138e86457780c6d9980f0a`
- author/committer: `rafaelmeloreisnovo`
- added compilation and execution of forensic-time contract tests.

### Authorship-scope refinement

Commit `3885e67431fff77ce21ce15be746e91016dd2f8f`
- author/committer: `rafaelmeloreisnovo`
- added `tools/check_rmr_headers.py` to workflow path triggers.
- replaced repository-wide header verification with:
  - `--root rmr/crypto`
  - `--root rmr/PROVENIENCE.md`

### Exact-head checkout

Commit `470b6d4037af046b65c7a7bdde7230f6769de29e`
- author/committer: `rafaelmeloreisnovo`
- set checkout ref to:
  `${{ github.event.pull_request.head.sha || github.sha }}`.

## Observed failure on commit 29b1a651...

Workflow run: `36286488972`
Job: `Validate RVC1 ZIP custody contract`

Observed PASS evidence before the final failure:
- `crc32c_kat=PASS`
- `zip_crc32_kat=PASS`
- `sha256_kat=PASS`
- `primary_source_markers=PASS`
- `RMR_ZIP_CUSTODY=PASS`
- seven ZIP-custody unit tests: OK
- ten forensic-time tests: OK

The later step `Verify ZIP custody authorship boundary` failed with:

`MISSING_HEADER: rmr/crypto/runtime/receipts/RMR_CRYPTO_V3_PROVIDER_CROSSCHECK_20260924.json`

## Classification

The recorded job failure was not a failure of the ZIP CRC/SHA KATs or the forensic-time contract tests. It was a later governance/authorship-header gate failure.

Therefore a UI that reports only early test steps as PASS can still precede a terminal workflow FAIL.

## R3

- F_ok: ZIP custody evolution and terminal failing step identified.
- F_gap: canonical synthesis not yet written.
- F_next: consolidate provenance, runtime evidence, responsibility boundaries, and corrective recommendations into the canonical manifesto.
