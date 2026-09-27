# μ0006 — CF140 ELF gate history and current failure

- date: 2026-09-27
- clock: TOKEN_VAZIO
- parent_commit: 0ea4b41801e8a39fa4c5a0689f5bd818ead23102
- kind: SOURCE_PLUS_EXECUTION_EVIDENCE
- claim_allowed: false

## Creation

Commit `1bcc39e1f7ecaa04f9615516e4300cc3d60bf18f`
- author/committer: `rafaelmeloreisnovo`
- created `.github/workflows/rmr-crypto-freestanding140-v1.yml`.
- created `rmr/crypto_freestanding140/audit/audit_artifact.sh`.
- initial artifact audit rejected:
  - `PT_INTERP`
  - `DT_NEEDED`
  - any `readelf -Ws` row whose section index field equals `UND`.

## Fail-closed modification

Commit `b663e43bfc7070c749d076810b3c0147f4e8b278`
- author/committer: `rafaelmeloreisnovo`
- subject: `fix(rmr): make CF140 undefined-symbol audit fail closed`.
- changed the undefined-symbol pipeline so that any matching `UND` row causes explicit:
  `FAIL unexpected_UND`
  followed by exit 1.

The predicate remained:

`awk '$7=="UND" {bad=1; print} END{exit bad}'`

No exclusion for the reserved symbol-table entry at index 0 was added.

## Current execution evidence

Commit: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
Workflow run: `36287272138`
Job: `freestanding140` / `108530284178`

Observed sequence:
- LLD provisioning: success.
- hosted semantic selftest: success.
- `PASS CF140_HOST_SELFTEST`.
- `PASS CF140_SHA256_ABC_COMPRESSION_KAT`.
- `PASS CF140_SOURCE_SHAPE`.
- cross-link script invokes clang/lld with `-Wl,--no-undefined`.
- artifact auditor prints:
  `0: 0000000000000000 0 NOTYPE LOCAL DEFAULT UND`
- auditor then emits:
  `FAIL unexpected_UND`
- job exits 1.

## External specification cross-check

TIS/System V ELF specification states that symbol-table index 0 is the reserved `STN_UNDEF` entry. Its canonical fields include:
- no name;
- zero value;
- zero size;
- no type;
- local binding;
- `SHN_UNDEF`.

Reference consulted:
`https://refspecs.linuxfoundation.org/elf/elf.pdf`

Therefore the exact row observed in the job is compatible with the mandatory/reserved ELF symbol-table entry itself. The current predicate does not distinguish that reserved row from a named unresolved external symbol.

## Classification

Current evidence supports:
- KAT failure: NOT_OBSERVED.
- source-shape failure: NOT_OBSERVED.
- named unresolved external symbol: NOT_PROVEN by the failing row.
- audit predicate false positive on `STN_UNDEF #0`: SUPPORTED_BY_EXECUTION_AND_ELF_SPEC.

No code correction is made in this entry; this ledger preserves the pre-fix state.

## R3

- F_ok: source commit, tightening commit, runtime row, and ELF specification linked.
- F_gap: separate historical ZIP-custody late gate remains to be classified.
- F_next: record ZIP custody gate history and the missing-header failure.
