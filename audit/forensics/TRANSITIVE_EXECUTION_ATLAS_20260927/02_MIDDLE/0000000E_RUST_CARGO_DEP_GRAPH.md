# μTRACE 0x0000000E — OBSERVE / Rust-Cargo resolver and executable dependency map

parent_mu=0x0000000D
phase=OBSERVE
function=CONTROL_GRAFO
procedure_id=RUSTCARGO-0003
timestamp_local=2026-09-27T21:31-03:00
evidence_state=OBSERVED
claim_allowed=false

## Toolchain control
No rust-toolchain or rust-toolchain.toml exists in audited tree.
Toolchain selection therefore comes from workflow/action/runner/user environment rather than a repository toolchain file.
.cargo/config.toml is present and can affect Cargo execution (including wasm runner previously recorded).

## Build-script execution
Three repository build.rs files:
- /build.rs
- c/blake3_c_rust_bindings/build.rs
- tools/compiler_version/build.rs
Cargo executes applicable build scripts as part of builds.

## Lock boundary
Only b3sum/Cargo.lock exists in audited tree.
Root library has no committed root Cargo.lock.

b3sum lock parsed:
67 packages total; registry entries carry checksums.

Selected locked graph:
blake3 1.8.7 local
 -> cc 1.4.3 -> find-msvc-tools, shlex
 -> constant_time_eq 0.4.2
 -> cpufeatures 0.3.0 -> libc
 -> memmap2 0.9.11 -> libc
 -> rayon-core 1.13.0 -> crossbeam-deque, crossbeam-utils

CLI macro path:
clap_derive 4.6.4
 -> proc-macro2 1.0.107
 -> quote 1.0.47
 -> syn 3.0.3
Proc-macro dependencies participate in compile-time code generation/execution semantics and therefore belong in the custody graph, even though they are not ordinary runtime libraries.

## Exactness distinction
Cargo.toml requirement rayon-core="1.12.1" resolved in b3sum lock to rayon-core 1.13.0.
Cargo.toml build requirement cc="1.1.12" resolved in b3sum lock to cc 1.4.3.
This is normal compatible semver resolution and proves why manifest text alone is insufficient to reconstruct the executable dependency set.

## Native boundary
cpufeatures/memmap2 lead to libc in the locked CLI graph.
cc build dependency leads from Rust build orchestration into external/native compiler selection.
Thus:
Rust source -> Cargo resolver -> build.rs/proc-macro -> compiler/native libs is an OBSERVED intertwined chain.

ROOT_EXACT_DEP_GRAPH=TOKEN_VAZIO
B3SUM_EXACT_LOCK_GRAPH=OBSERVED
PROC_MACRO_COMPILE_TIME_EDGE=OBSERVED
NATIVE_LIBC_EDGE_IN_LOCKED_CLI_GRAPH=OBSERVED
MALICIOUS_DEPENDENCY_BEHAVIOR=NOT_PROVEN

syslog=OBSERVE|0x0000000E|RUSTCARGO-0003|resolver_and_executable_dependency_map|next=historical_run_environment_and_action_resolution
