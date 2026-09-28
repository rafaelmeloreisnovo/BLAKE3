# μ0001 — Source snapshot

- date: 2026-09-27
- clock: TOKEN_VAZIO
- repository: rafaelmeloreisnovo/BLAKE3
- branch_under_audit: master
- audited_head: b1c8cab1d75943a628e447fa72bb06ebeb348489
- ledger_branch: audit/check-custody-manifest-20260927
- parent_commit: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: SOURCE_SNAPSHOT
- claim_allowed: false

## Sources read

1. `.github/workflows/ci.yml`
2. `.github/workflows/rmr-full-validation.yml`
3. `.github/workflows/rmr-upstream-comprehensive-v3.yml`
4. `.github/workflows/rmr-crypto-freestanding140-v1.yml`
5. `rmr/crypto_freestanding140/audit/audit_artifact.sh`
6. `.github/workflows/rmr-zip-custody.yml`
7. GitHub commit history for each path above.
8. GitHub check-runs for commits `b1c8cab1...`, `29b1a651...`, and `69cdd02f...`.
9. Workflow/job logs for the observed late FAIL states.

## Current facts at audited head

- `master` branch protection: disabled.
- repository rulesets returned: none.
- required status checks on `master`: none.
- therefore a GitHub Actions failure observed after a push is not, by itself, evidence that the server rejected that push.
- commit `b1c8cab1...` exposed 92 check-runs when queried with complete pagination; 91 concluded success and one concluded failure.
- the failing check was `freestanding140`.

## Evidence rule

`SOURCE != EXECUTION != EVIDENCE != CLAIM`

This entry records only facts observed from repository state and GitHub execution records. Responsibility is attributed only as commit authorship/committer identity where GitHub records it; motive and intent remain TOKEN_VAZIO.

## R3

- F_ok: source set fixed and audited head fixed.
- F_gap: historical deltas not yet classified.
- F_next: classify upstream CI provenance separately from RMR workflow provenance.
