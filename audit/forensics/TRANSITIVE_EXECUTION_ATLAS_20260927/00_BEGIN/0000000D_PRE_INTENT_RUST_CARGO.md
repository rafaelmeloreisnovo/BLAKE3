# μTRACE 0x0000000D — PRE_INTENT / Rust-Cargo custody descent

parent_mu=0x0000000C
phase=PRE_INTENT
function=CONTROL_GRAFO
procedure_id=RUSTCARGO-0003
timestamp_local=2026-09-27T21:31-03:00
claim_allowed=false

INTENT:
Descend Rust/Cargo from control plane to executable dependency plane.

Planned checks:
A. toolchain acquisition/control: rust-toolchain files, Cargo config, workflow rustup/action inputs.
B. resolver custody: lockfiles, workspace/package boundaries, registry source/checksum, git/path dependencies.
C. executable dependency classes: build-dependencies, proc-macro crates, build.rs, compiler wrappers/config.
D. native bridge: cc crate -> CC/CFLAGS/AR and C/ASM -> linker.
E. runtime/native libs: mmap/rayon/system libraries where enabled.
F. historical/runtime values remain TOKEN_VAZIO unless run evidence establishes them.

No source modification will be made. Findings are forensic inventory only.

syslog=PRE_INTENT|0x0000000D|RUSTCARGO-0003|rust_cargo_descent|next=enumerate_toolchain_and_manifest_control
