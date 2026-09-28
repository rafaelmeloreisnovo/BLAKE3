# μ0012 — RMR import, fetch, and execution graph

- date: 2026-09-27
- parent_commit: f8d421bf8f22027e45d74ceddc08e499245275c2
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: TRANSITIVE_EXECUTION_GRAPH
- method: bottom-up static expansion of observed workflow children
- claim_allowed: false

## Rule

This receipt distinguishes indirect execution from concealment.

`INDIRECTION != OBFUSCATION`
`MUTABLE_INPUT != MALICIOUS_INPUT`
`OBSERVED_EDGE != INTENT`

## Shared shell-library edge

Observed RMR scripts including:
- `rmr/tools/benchmark_rust_cli_v3.sh`;
- `rmr/tools/audit_source_delta_v3.sh`;
- `rmr/tools/build_cross_arch_blake3_v3.sh`

source:

`rmr/upstream_validation/common.sh`

Therefore the visible workflow command is not the complete execution description:

`workflow -> RMR script -> source common.sh -> functions/environment -> child commands`

## common.sh external-source resolver

Observed defaults:

`OFFICIAL_REPO=${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}`

`OFFICIAL_REF=${OFFICIAL_REF:-6aab490a26124663329dfd3961b8469f8fdb158b}`

Function `rmr_checkout_official` performs:
1. conditional `git clone --filter=blob:none --no-tags`;
2. `git fetch --depth 1 origin "$OFFICIAL_REF"`;
3. detached checkout of `FETCH_HEAD`;
4. `rev-parse HEAD`.

The default ref is a full commit SHA. This constrains the intended upstream Git object more strongly than a moving branch/tag. Network transport, GitHub availability, and Git implementation remain runtime inputs.

## Override surfaces

`OFFICIAL_REPO` and `OFFICIAL_REF` are environment-overridable.

Other observed execution selectors include:
- `CC`;
- `CXX`;
- `CFLAGS`;
- `WORK_ROOT`;
- `RESULT_ROOT`;
- benchmark sizing/round variables;
- Cargo/target variables documented in μ0010.

Existence of an override surface is not evidence that an unauthorized override occurred.

Current classification:
- override capability: OBSERVED
- workflow-level unauthorized override: NOT_OBSERVED
- external redirection event: NOT_PROVEN

## Direct orchestrator fetch

`rmr/tools/orchestrate_blake3_compare_v2.sh` independently defines the same official repository/ref defaults and directly performs clone/fetch/checkout before compiling both official and fork trees.

Observed chain:

`workflow -> orchestrate_blake3_compare_v2.sh -> git clone/fetch official SHA -> CMake official -> CMake fork -> binaries -> benchmark/analyzer -> receipt`

This is a network tail below a locally stored script call.

## Local-clone edge

`benchmark_rust_cli_v3.sh` also performs:

`git clone --local "$RMR_REPO_ROOT" "$FORK_ROOT"`

followed by detached checkout of the current fork HEAD.

This is local Git object reuse, distinct from the upstream network clone.

## Native compiler tails

`build_cross_arch_blake3_v3.sh` expands into repeated Clang invocations for portable/SIMD source sets and targets including x86, ARMv7, AArch64 and WASM-related validation paths.

Therefore a single workflow step can fan out into many compiler processes.

## Source-root shadow

As recorded in μ0010, the C Rust-bindings build script supports `BLAKE3_C_DIR_OVERRIDE`. This is a source-root indirection boundary and must be captured in any hermetic execution receipt.

## Forensic conclusion

Observed:
- shell import indirection: YES
- script-to-network Git edge: YES
- environment-selectable upstream endpoint/ref: YES
- full-SHA default upstream ref: YES
- local detached clone of fork HEAD: YES
- compiler fan-out: YES
- encrypted command body: NOT_OBSERVED
- encoded command decoder: NOT_OBSERVED
- deliberate concealment: NOT_ESTABLISHED

The factual description supported by the inspected sources is **transitive execution and supply-chain indirection**.

## R3

- F_ok: RMR source/import/fetch/compile edges expanded.
- F_gap: CMake package-resolution and generated-command shadows remain to be isolated.
- F_next: map CMake/TBB/package discovery and distinguish configured OFF paths from runtime-enabled paths.
