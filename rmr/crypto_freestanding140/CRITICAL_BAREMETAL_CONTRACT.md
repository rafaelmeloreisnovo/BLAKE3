# RMR Crypto Freestanding 140 — Critical Bare-Metal Guard V1

## Boundary

This layer supervises execution; it does not change BLAKE3 or any standard cryptographic primitive.

```text
CRYPTO_SEMANTICS != SUPERVISOR_POLICY
COMPILE_GATE != PHYSICAL_HARDWARE_PASS
ROLLBACK_ACTION != ROLLBACK_EXECUTED
WATCHDOG_EVENT != HARDWARE_WATCHDOG_DRIVER
```

The Rust BLAKE3 library already supports `no_std` when the `std` feature is disabled. The canonical RMR commands are exposed as Cargo aliases:

```text
cargo rmr-freestanding
cargo rmr-baremetal
```

`rmr-baremetal` targets `thumbv7em-none-eabi`, disables default features and selects `pure`. This is a compile gate for an OS-less target; it is not a physical-device execution claim.

## Cargo lock boundary

The repository intentionally ignores `Cargo.lock` because the root is a library crate. The RMR gate must not pretend a persistent lock exists.

For each gate execution, when no lock file is present, `build_rust_baremetal.sh` creates one ephemeral `Cargo.lock`, prints its SHA-256, executes all Rust checks with `--locked`, and removes the ephemeral lock on exit. Therefore:

```text
LOCKED_WITHIN_RUN = true
LOCK_PERSISTED_IN_REPO = false
CROSS_RUN_DEPENDENCY_REPRODUCIBILITY = TOKEN_VAZIO
```

A persistent supply-chain lock is a separate governance decision and is not introduced by this guard.

## Fail-ACK protocol

Execution is not committed merely because the operation reports success.

```text
IDLE --ARM--> RUNNING
RUNNING --EXEC_OK--> ACK_WAIT
ACK_WAIT --ACK_OK--> IDLE + COMMIT
```

Until `ACK_OK`, success remains uncommitted.

## Failure and watchdog events

The supervisor owns no clock and no hardware timer. A board/platform layer may report `WATCHDOG_EXPIRED` as an explicit event.

`EXEC_FAIL`, `ACK_FAIL`, or `WATCHDOG_EXPIRED` consume a bounded retry budget. The budget is caller-selected but capped by `RMR_CF_GUARD_RETRY_LIMIT`.

When the budget is exhausted:

```text
... -> ROLLBACK_ACTION
ROLLBACK_OK -> FAILSAFE
ROLLBACK_FAIL -> HALTED
```

A successful rollback does not automatically resume execution. `OPERATOR_RESET` is required to leave `FAILSAFE`.

Unknown state/event combinations fail closed into `FAILSAFE`; a null supervisor pointer returns `HALT`.

## Adaptive boundary

Adaptation is restricted to a deterministic, bounded retry decision based on explicit events and the configured retry budget. It does not:

- change cryptographic semantics;
- discover new objectives;
- silently downgrade checks;
- infer hardware success;
- invent an ACK;
- bypass rollback/failsafe states.

## Evidence gates

| Gate | Meaning |
|---|---|
| `CF140_GUARD_FAIL_ACK_WATCHDOG_SELFTEST` | hosted state-machine paths executed |
| cross-matrix static link | supervisor compiled/linked without hosted runtime on declared ABI targets |
| `RMR_RUST_FREESTANDING_NO_STD_PURE` | Rust library checks with no default features + `pure` |
| `RMR_BAREMETAL_REJECTS_DEFAULT_STD` | negative gate: default hosted profile is rejected on the OS-less target for the expected `std` reason |
| `RMR_RUST_BAREMETAL_COMPILE_GATE` | Rust library checks for `thumbv7em-none-eabi` with no default features + `pure` |
| physical watchdog/reset/flash rollback | `NOT_RUN` until board-specific driver and hardware receipt exist |

No `PASS` above may be promoted from source presence alone.
