# RMR CF140 receipt — Rust gate lock precondition fix

- timestamp_utc: 2026-10-02T03:39:00Z
- parent_commit: `3486a06a409c7ae038138cf503028bbfc11f64fa`
- parent_pr: `#171`
- parent_run: `36961062970`
- failed_job: `rust-baremetal-gate`
- C_freestanding140_job: `PASS`
- Rust_baremetal_job: `FAIL`
- physical_hardware: `NOT_RUN`

## Observed defect

The initial RMR Cargo aliases required `--locked`, while this library repository contains no `Cargo.lock` and `.gitignore` explicitly ignores `Cargo.lock`. The test gate therefore imposed a lock precondition that the repository policy does not provide.

This receipt does not reinterpret the failed CI run as a cryptographic or bare-metal failure. It records a gate-orchestration defect.

## Superseding change

The aliases no longer hard-code `--locked`. `build_rust_baremetal.sh` now:

1. preserves any pre-existing lock;
2. otherwise creates one ephemeral lock for the run;
3. emits its SHA-256 when a hashing utility is available;
4. applies `--locked` to all subsequent Rust checks;
5. removes only the lock it created.

## Claim boundary

```text
FIRST_RUN = FAIL
FAIL_SCOPE = TEST_ORCHESTRATION
BAREMETAL_COMPILE = NOT_RUN_BY_SUCCESSFUL_GATE
PHYSICAL_EXECUTION = NOT_RUN
CLAIM_ALLOWED = false until superseding CI completes successfully
```
