# μ0004 — Cancelled producers -> late aggregate FAIL

- date: 2026-09-27
- clock: TOKEN_VAZIO
- parent_commit: 4e08283e8b264cecf7d9929a841474a734f8309a
- kind: EXECUTION_EVIDENCE
- claim_allowed: false

## Configuration facts

Current `rmr-full-validation.yml`:
- concurrency is grouped by PR number or ref.
- `cancel-in-progress: true`.
- final job `aggregate` uses `if: always()`.
- `aggregate` depends on producer jobs and downloads `rmr-full-*` artifacts.
- the evidence gate asserts `execution_complete is True` and `validation_state == COMPLETE`.

Current `rmr-upstream-comprehensive-v3.yml`:
- same PR/ref supersession principle.
- `cancel-in-progress: true`.
- final job `summary` uses `if: always()`.
- `summary` depends on nine producer jobs and downloads `upstream-v3-*` artifacts.
- the evidence gate asserts complete execution.
- a later gate also requires every `needs.<job>.result` to equal `success`.

## Runtime evidence on commit 29b1a651...

### Full validation

Run: `36286488994`
Final job: `aggregate` / job `108530285150`

Observed:
- producer jobs were cancelled after the later master push.
- `aggregate` still executed.
- log: `Found 0 artifact(s)`.
- report: `RMR_FULL_VALIDATION_EXECUTION_COMPLETE=false`.
- report: `RMR_FULL_VALIDATION_STATE=PARTIAL`.
- required axes became `TOKEN_VAZIO_NOT_FOUND`.
- final assertion raised `AssertionError`.
- job concluded failure.

### Comprehensive V3

Run: `36286489003`
Final job: `summary` / job `108530286155`

Observed:
- producer jobs were cancelled after the later master push.
- `summary` still executed.
- log: `Found 0 artifact(s)`.
- report: `RMR_UPSTREAM_COMPREHENSIVE_EXECUTION_COMPLETE=false`.
- report: `RMR_UPSTREAM_COMPREHENSIVE_STATE=PARTIAL`.
- required axes became `TOKEN_VAZIO_NOT_FOUND`.
- final assertion raised `AssertionError`.
- job concluded failure.

## Classification

This evidence supports the following execution chain:

`new push -> previous run superseded/cancelled -> final job runs because always() -> no producer artifacts -> PARTIAL/TOKEN_VAZIO -> completeness assertion -> FAIL`

The final FAIL is a valid representation of incomplete evidence under the present gate, but it is not equivalent to evidence that an earlier producer test found a defect.

Recommended semantic distinction for future implementation:
- `FAIL_TEST`
- `CANCELLED_SUPERSEDED`
- `EVIDENCE_INCOMPLETE`

No change is applied by this ledger entry.

## R3

- F_ok: late FAIL mechanism reproduced from workflow and logs.
- F_gap: check enumeration/pagination can still hide a later failing check.
- F_next: record complete check-run enumeration behavior.
