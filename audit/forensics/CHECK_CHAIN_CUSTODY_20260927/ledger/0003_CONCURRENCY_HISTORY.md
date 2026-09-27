# μ0003 — Concurrency policy history

- date: 2026-09-27
- clock: TOKEN_VAZIO
- parent_commit: 9615d76b24681a99a213c311002f9bbf805eb010
- kind: HISTORICAL_DELTA
- scope:
  - .github/workflows/rmr-full-validation.yml
  - .github/workflows/rmr-upstream-comprehensive-v3.yml
- claim_allowed: false

## Chronology observed from Git diffs

### 2026-09-23 — initial RMR matrices

`978e1362ac61c95ad8623555f724da577afa3d19`
- author/committer: `rafaelmeloreisnovo`
- created `rmr-full-validation.yml`
- initial concurrency group:
  `rmr-full-validation-${{ github.event.pull_request.number || github.ref }}`
- `cancel-in-progress: true`
- final `aggregate` already used `if: always()`.

`dd8fe7174c76ab95b47b3a12b7bd179dd9271d15`
- author/committer: `rafaelmeloreisnovo`
- created `rmr-upstream-comprehensive-v3.yml`
- initial concurrency group:
  `rmr-upstream-comprehensive-v3-${{ github.event.pull_request.number || github.ref }}`
- `cancel-in-progress: true`
- final `summary` already used `if: always()`.

### 2026-09-23 — global supersession

`141f8065e2c3845978cff7aabb6eeadebc155d30`
- GitHub author: `rafaelmeloreisnovo`
- GitHub committer: `web-flow`
- full validation changed:
  `...-${{ PR number || ref }}` -> `rmr-full-validation-global`
- comprehensive changed:
  `...-${{ PR number || ref }}` -> `rmr-upstream-comprehensive-v3-global`

Observable effect of the diff: unrelated refs could share one global concurrency group.

### 2026-09-24 — exact-head isolation

`603fdda064d919521cdebeea8ce1c8a760795cf0`
- author/committer: `rafaelmeloreisnovo`
- appended `${{ github.event.pull_request.head.sha || github.sha }}` to concurrency groups across multiple RMR workflows.
- full validation became:
  `rmr-full-validation-global-${{ head SHA || github.sha }}`
- comprehensive became:
  `rmr-upstream-comprehensive-v3-global-${{ head SHA || github.sha }}`

Observable effect of the diff: distinct commit SHAs no longer share the same concurrency group, so a newer SHA does not supersede an older SHA merely by sharing the branch.

### 2026-09-24 — PR/ref supersession restored

`1f44afd6c7ba043ace007905f0904b13b68a8d5e`
- author/committer: `rafaelmeloreisnovo`
- full validation changed from global+SHA to:
  `rmr-full-validation-${{ github.event.pull_request.number || github.ref }}`

`12c48f0a405046abe0079b472785869b3fb5eb8a`
- author/committer: `rafaelmeloreisnovo`
- comprehensive changed from global+SHA to:
  `rmr-upstream-comprehensive-v3-${{ github.event.pull_request.number || github.ref }}`

Both retained:
`cancel-in-progress: true`

## Runtime evidence matching the current policy

For master commit `29b1a651...`:

- RMR full validation run `36286488994`: later concluded `cancelled`.
- RMR comprehensive run `36286489003`: later concluded `cancelled`.
- several producer jobs completed as `cancelled` at approximately `2026-09-27T02:01:11Z`.

The next master push for `b1c8cab1...` created new runs at approximately `2026-09-27T02:01:10Z`.

This time adjacency plus the current identical branch/ref concurrency key is evidence consistent with supersession by the newer master push.

## Responsibility boundary

The commit records establish who authored/committed each configuration change. They do not establish motive. The observed late cancellation is an execution consequence of the current configuration plus the subsequent push.

## R3

- F_ok: concurrency evolution reconstructed.
- F_gap: late aggregate/summary FAIL semantics not yet isolated.
- F_next: record how `if: always()` transforms cancelled producers into a later failing gate.
