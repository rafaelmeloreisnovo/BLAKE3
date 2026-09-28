# μ0005 — Check-run enumeration and pagination

- date: 2026-09-27
- clock: TOKEN_VAZIO
- parent_commit: b25f2eec444799bb3cefffe4f12acc8c05b71f13
- kind: EVIDENCE_ENUMERATION
- audited_commit: b1c8cab1d75943a628e447fa72bb06ebeb348489
- claim_allowed: false

## Observation

A check-runs query without explicit complete pagination returned a response with:

- `total_count = 92`
- only 30 check-run objects present in that response page
- all 30 returned objects concluded `success`

A subsequent query with `per_page=100` returned all 92 check-runs.

Complete enumeration showed:

- 91 `success`
- 1 `failure`

The failing check was:
- workflow: `rmr-crypto-freestanding140-v1`
- job/check: `freestanding140`
- run: `36287272138`
- job: `108530284178`

## Additional API boundary

The commit combined-status endpoint returned no legacy status contexts for this audited commit, while the Checks API contained the GitHub Actions check-runs.

Therefore:

`legacy commit statuses != GitHub Actions check-runs`

and:

`first page of check-runs != complete commit check state`

## Required evidence rule for tooling

A consumer such as RafGitTools must not promote a commit to complete PASS unless:

1. all check-run pages have been enumerated;
2. returned count equals the authoritative `total_count` for the snapshot, or pagination exhaustion is otherwise proven;
3. all relevant checks have reached terminal states;
4. failures, cancellations, skipped states, and superseded runs remain distinguishable;
5. legacy statuses and Checks API results are not conflated.

## R3

- F_ok: hidden-check mechanism identified as incomplete enumeration, not hidden repository variable.
- F_gap: cause of the sole current `freestanding140` failure still needs source-level classification.
- F_next: trace the ELF undefined-symbol gate history and execution.
