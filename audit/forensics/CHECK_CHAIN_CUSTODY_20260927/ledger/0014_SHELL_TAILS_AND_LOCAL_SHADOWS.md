# μ0014 — Shell tails, eval boundary, and local source shadows

- date: 2026-09-27
- parent_commit: 36bc032f0747c57da8b48c59224d1363a887e436
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: SHELL_TRANSITIVE_GRAPH
- shell_files_enumerated: 70
- claim_allowed: false

## Coverage

All 70 `.sh` blobs in the audited Git tree were enumerated for shell imports/source, script-to-script calls, Git clone/fetch, compiler/linker invocations, Cargo/CMake/Make children, Python children/heredocs, command substitutions, artifact/symbol tooling, and network-resolution edges visible in shell.

This is an execution-edge scan, not a proof of semantic correctness of every shell statement.

## Shared RMR source tail: profiles.mk

Multiple `rmr/build/*.sh` files perform:

`. "$(dirname "$0")/profiles.mk"`

`profiles.mk` defines default profile `throughput`, base C flags `-ffreestanding -fno-stack-protector`, base link flags `-nostdlib -Wl,-e,_start -pie`, four named profiles, and external extension surfaces `RMR_EXTRA_CFLAGS` / `RMR_EXTRA_LDFLAGS`.

### eval boundary

`rmr_select_profile()` uses shell `eval` to select the named profile variables.

Before eval, `profile` is constrained by a case statement to `latency|throughput|deterministic|debug`.

Therefore:
- use of shell `eval`: OBSERVED;
- profile-name arbitrary injection through this path: NOT_SUPPORTED_BY_CURRENT_SOURCE, due allowlist;
- environment-driven compiler/linker option extension through `RMR_EXTRA_CFLAGS/LDFLAGS`: OBSERVED.

This is a configuration shadow, not evidence of encrypted command execution.

## Non-tree local-source shadow

`rmr/build/build_blake3_omega.sh` compiles:
- `BLAKE3-master/c/blake3.c`;
- `BLAKE3-master/c/blake3_dispatch.c`;
- `BLAKE3-master/c/blake3_portable.c`;
- optionally `BLAKE3-master/c/blake3_neon.c`.

It also includes `-IBLAKE3-master/rmr/include -IBLAKE3-master/c`.

No `BLAKE3-master/` directory exists in the audited tree.

Classification:
- source path referenced: OBSERVED;
- source bytes in audited commit: ABSENT;
- if path absent at runtime: build fails;
- if path exists externally: build can consume source outside audited Git tree;
- provenance of such external local directory: TOKEN_VAZIO until runtime receipt.

This is a materially important local shadow.

## Mutable upstream clone paths

### tools/benchmark_compare_official.sh

Performs a shallow clone of `https://github.com/BLAKE3-team/BLAKE3` with no explicit ref/commit checkout.

Classification: `UPSTREAM_REFERENCE = CURRENT_DEFAULT_BRANCH_HEAD_AT_RUNTIME`.

### tools/cmake_ci_compare.sh

Defaults to `OFFICIAL_REF=""`. With `--clone-official` and no `--official-ref`, it shallow-clones current default HEAD. A ref is only constrained when explicitly supplied by the caller.

### Pinned comparison paths

Several RMR V3 scripts use `common.sh`, whose default `OFFICIAL_REF` is the full SHA `6aab490a26124663329dfd3961b8469f8fdb158b`.

## Shell import graph

A recurring imported library is `rmr/upstream_validation/common.sh`.

Observed consumers include:
- `audit_abi_elf_v3.sh`;
- `audit_source_delta_v3.sh`;
- `benchmark_backends_v3.sh`;
- `benchmark_rust_cli_v3.sh`;
- `benchmark_tbb_v3.sh`;
- `build_cross_arch_blake3_v3.sh`;
- `run_c_ablation_v3.sh`;
- `validate_c_quality_v3.sh`.

This library performs the pinned upstream clone/fetch and therefore propagates a network tail to every consumer.

## High fan-out integration shell

`validate_rmr_integration_v3.sh` calls Python validators, the crypto contract matrix, CMake configure/build, HWIF cross builds, fixed256 probes, freestanding custody builders, PAI42 tests, and topology audit.

A single workflow line invoking this script therefore expands into many child processes and source domains.

## Negative findings from shell enumeration

- no encrypted payload decoder was identified;
- no base64-to-shell execution chain was identified;
- no `curl|bash` was found inside RMR shell scripts in this enumeration; the known remote installer pipes reside in base CI YAML;
- no versioned Git hook installer was identified.

## Forensic classification

- shell tails: OBSERVED
- shell `source` indirection: OBSERVED
- allowlisted `eval`: OBSERVED
- environment compiler/linker control: OBSERVED
- non-tree source-root reference: OBSERVED / HIGH-PROVENANCE-GAP
- mutable upstream HEAD clone: OBSERVED
- deliberate concealment: NOT_ESTABLISHED
- encrypted command methodology: NOT_OBSERVED

## R3

- F_ok: all shell files enumerated; strongest local/external shadows isolated.
- F_gap: Python process/network/file-loader behavior not yet exhaustively enumerated.
- F_next: scan every Python file for subprocess, imports, dynamic execution, network, archive extraction, and filesystem-controlled code loading.