# RMR CF140 — Cargo Freestanding Matrix Receipt V1

Date: 2026-10-02
Repository: `rafaelmeloreisnovo/BLAKE3`
PR: `#172`
Evidence run: `36973317189` / run `#34`
Evidence job: `rust-baremetal-gate` / `110731714760`
Head under test: `450e32a335d118c28e83c846bfb67a370d38cd34`

## Boundary

```text
SOURCE != EXECUTION != EVIDENCE != CLAIM
CHECK_PASS != BUILD_PASS
BUILD_PASS != FINAL_LINK_PASS
BUILD_PASS != HARDWARE_PASS
```

## Toolchain

```text
rustc 1.99.0 (b940084d7 2026-09-28)
rustc commit: b940084d7eb6a299eb4bfeb8e34901bc051e7ac4
LLVM: 23.1.1
cargo 1.99.0 (5f94df478 2026-08-27)
```

## Locked execution evidence

```text
RMR_CARGO_LOCK_SHA256=d352ec1e33e3b24d22cdd4e06a952c5cb674c024f8ab09d3d34358ccd57d9bd8
RMR_CARGO_METADATA_SHA256=f1083f7404f58919a0bd14941746df7ae3da9fd0c4159bbb839905dcd7b11943
PASS RMR_RUST_FREESTANDING_NO_STD_PURE_BUILD
```

The `Cargo.lock` was generated only for the CI execution, used with `--locked`, and removed by the gate because the root library policy does not persist `Cargo.lock`.

## OS-less build matrix

| Target | default `std` negative gate | `no_std + pure` build | RLIB SHA-256 |
|---|---|---|---|
| `thumbv7em-none-eabi` | PASS | PASS | `e7288227e35f8bb0ac3ef8a470906b7a7d3c174ee9d7e642fb2faec68d8c7339` |
| `armv7a-none-eabi` | PASS | PASS | `b93c2ca98fb3286f5d2a59f38bf20cbe2440a91dcf91a96d556bb82fab40fb8c` |
| `aarch64-unknown-none` | PASS | PASS | `37bc7339ccdd62ca47adbbb30ed16fdb0a4d8e6f64edccb99ee1dadf6ba5b64d` |

Final marker:

```text
PASS RMR_RUST_BAREMETAL_MATRIX targets=thumbv7em-none-eabi armv7a-none-eabi aarch64-unknown-none
```

The sibling `freestanding140` job `110731714549` also completed `SUCCESS`, including hosted semantic selftest and C cross-link/source/binary gates.

## Claim state

```text
RUST_NO_STD_PURE_BUILD = PASS
RUST_OSLESS_M_BUILD = PASS
RUST_OSLESS_A32_BUILD = PASS
RUST_OSLESS_A64_BUILD = PASS
RUST_RLIB_IDENTIFIED = PASS
FINAL_OSLESS_EXECUTABLE_LINK = TOKEN_VAZIO
PHYSICAL_BAREMETAL_EXECUTION = NOT_RUN
PHYSICAL_WATCHDOG_RESET_FLASH_ROLLBACK = NOT_RUN
CROSS_RUN_DEPENDENCY_REPRODUCIBILITY = TOKEN_VAZIO
```

No hardware or final executable-link claim is promoted from these library-build results.

## R3

`R3=<F_ok: Cargo check promoted to build; pinned Rust; M/A32/A64 OS-less library artifacts produced and hashed; negative std gates PASS, F_gap: final OS-less executable link TOKEN_VAZIO; physical execution NOT_RUN; persistent cross-run lock TOKEN_VAZIO, F_next: do not expand this Cargo scope; only a separately authorized bounded linker/board receipt may promote the remaining states>`
