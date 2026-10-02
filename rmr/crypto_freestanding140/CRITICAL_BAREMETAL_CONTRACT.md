# RMR Crypto Freestanding 140 — Critical Bare-Metal Guard V1

## Boundary

This layer supervises execution; it does not change BLAKE3 or any standard cryptographic primitive.

```text
CRYPTO_SEMANTICS != SUPERVISOR_POLICY
CHECK_PASS != BUILD_PASS
BUILD_PASS != LINK_PASS
COMPILE_GATE != PHYSICAL_HARDWARE_PASS
ROLLBACK_ACTION != ROLLBACK_EXECUTED
WATCHDOG_EVENT != HARDWARE_WATCHDOG_DRIVER
```

The Rust BLAKE3 library already supports `no_std` when the `std` feature is disabled. The canonical RMR aliases are:

```text
cargo rmr-freestanding
cargo rmr-baremetal       # compatibility alias: Cortex-M target
cargo rmr-baremetal-m     # thumbv7em-none-eabi
cargo rmr-baremetal-a32   # armv7a-none-eabi
cargo rmr-baremetal-a64   # aarch64-unknown-none
```

All RMR aliases above use `cargo build`, not `cargo check`, so the positive gate includes Rust code generation for the library artifact. These remain library-build gates; they are not final board-link or physical-execution claims.

The CI gate pins Rust `1.99.0` and validates the OS-less target matrix:

```text
thumbv7em-none-eabi
armv7a-none-eabi
aarch64-unknown-none
```

For each target, the gate first verifies that the default hosted `std` profile is rejected, then builds `--no-default-features --features pure`, requires a generated `libblake3-*.rlib`, and emits its SHA-256 into the CI evidence stream.

## Cargo lock boundary

The repository intentionally ignores `Cargo.lock` because the root is a library crate. The RMR gate must not pretend a persistent lock exists.

For each gate execution, when no lock file is present, `build_rust_baremetal.sh` creates one ephemeral `Cargo.lock`, prints its SHA-256, executes all Rust commands with `--locked`, and removes only the lock that it created. The gate also hashes the resolved `cargo metadata` output for the execution.

Therefore:

```text
LOCKED_WITHIN_RUN = true
LOCK_PERSISTED_IN_REPO = false
METADATA_HASH_RECORDED = true
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
| cross-matrix static link | C supervisor compiled/linked without hosted runtime on declared ABI targets |
| `RMR_RUST_FREESTANDING_NO_STD_PURE_BUILD` | Rust library codegen with no default features + `pure` |
| `RMR_BAREMETAL_REJECTS_DEFAULT_STD` | per-target negative gate: default hosted profile rejected on OS-less target |
| `RMR_RUST_BAREMETAL_BUILD_GATE` | per-target Rust library build for the OS-less M/A32/A64 matrix |
| `RMR_RLIB_SHA256` | generated Rust library artifact identified by target-specific SHA-256 |
| final OS-less executable link | `TOKEN_VAZIO` until a bounded linker/entry contract is authorized |
| physical watchdog/reset/flash rollback | `NOT_RUN` until board-specific driver and hardware receipt exist |

No `PASS` above may be promoted from source presence alone. `RLIB BUILD PASS` is stronger than `cargo check`, but it is still not final ELF link proof and not physical execution.
