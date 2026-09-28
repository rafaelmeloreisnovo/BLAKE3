# μTRACE 0x00000005 — MIDDLE / RMR sourced-script and upstream-fetch routes

parent_mu=0x00000004
phase=02_MIDDLE
source_ref=RMR scripts@5365b389919881f91be4a407ab5b2d094bc1a65c
evidence_state=OBSERVED
claim_allowed=false

## Shared tail
The following scripts source `rmr/upstream_validation/common.sh`:
- benchmark_rust_cli_v3.sh
- audit_source_delta_v3.sh
- build_cross_arch_blake3_v3.sh
- benchmark_backends_v3.sh
- run_c_ablation_v3.sh
- audit_abi_elf_v3.sh
- benchmark_tbb_v3.sh
- validate_c_quality_v3.sh

The sourced common.sh defines:
OFFICIAL_REPO default = BLAKE3-team/BLAKE3.git
OFFICIAL_REF default = 6aab490a26124663329dfd3961b8469f8fdb158b
and rmr_checkout_official() performs clone/fetch/checkout.

## Environment override surface
OFFICIAL_REPO and OFFICIAL_REF are shell-environment-overridable.
CC/CXX and several WORK/RESULT variables are also environment-selectable in downstream scripts.

This is an interface/control surface. No evidence in this record establishes hostile injection.

## Duplicate direct route
orchestrate_blake3_compare_v2.sh independently defines the same default official repo/ref and directly performs clone/fetch/checkout.

## Downstream execution
After acquisition, scripts invoke combinations of:
Cargo/build.rs, CMake, clang, Python, awk, pkg-config, nm, readelf, sha256sum, and produced binaries.

## Classification
YAML_TO_SCRIPT_INDIRECTION=OBSERVED
SCRIPT_TO_SOURCE_INDIRECTION=OBSERVED
SOURCE_TO_NETWORK_GIT=OBSERVED
ENVIRONMENT_OVERRIDE_EDGE=OBSERVED
UPSTREAM_SHA_PIN=OBSERVED
HIDDEN_FROM_IMMEDIATE_YAML_CALLSITE=YES
DELIBERATE_OBFUSCATION=NOT_PROVEN

## Direction
push/workflow -> RMR script -> common.sh -> Git network -> pinned upstream source -> build system -> compiler/linker -> produced binary/test

mu_record=0x00000005|0x00000004|02_MIDDLE|RMR scripts/common.sh|expand sourced/network tails|OBSERVED|dependency package closure and remote action internals pending|inspect Cargo lock/resolution boundary
