# μTRACE 0x00000006 — MIDDLE / Cargo library resolution boundary

parent_mu=0x00000005
phase=02_MIDDLE
source_ref=Cargo manifests/lock@5365b389919881f91be4a407ab5b2d094bc1a65c
evidence_state=OBSERVED
claim_allowed=false

## Root package
Root Cargo.toml contains semver dependency constraints and build-dependency cc="1.1.12".
No root Cargo.lock path exists in the audited Git tree.

Consequently, a root-package Cargo invocation that is not otherwise constrained by an external lock/cache/vendor policy has a dependency-resolution surface beyond the committed root manifest. Exact runtime resolution is TOKEN_VAZIO until execution metadata/cache/index state is captured.

## b3sum lock
b3sum/Cargo.lock:
- lock format version 4
- 67 parsed packages
- 65 registry-sourced packages
- 0 registry packages missing checksum in the parsed lock entries
Examples include exact locked cc=1.4.3, arrayvec=0.7.8, anyhow=1.0.104.

## Additional manifests
c/blake3_c_rust_bindings has build dependencies cc="1.0.48" and ignore="0.4.23".
tools/compiler_version has build dependency cc="1.0.50".
test_vectors depends on registry hex/serde/serde_json plus local path crates.

## Dependency semantics
SEMVER_CONSTRAINT != EXACT_RESOLVED_VERSION
LOCKED_CHECKSUM != VENDORED_SOURCE
REGISTRY_CHECKSUM = integrity evidence for locked crate payload, not proof of hermetic execution.
BUILD_DEPENDENCY can execute code during compilation through build scripts/proc-macros/tooling paths.

## Classification
ROOT_LOCKFILE=NOT_OBSERVED
B3SUM_LOCKFILE=OBSERVED
B3SUM_REGISTRY_CHECKSUMS=OBSERVED
ROOT_EXACT_RESOLUTION=TOKEN_VAZIO
DEPENDENCY_OBFUSCATION=NOT_PROVEN
DEPENDENCY_TRANSITIVE_EXECUTION_SURFACE=OBSERVED

mu_record=0x00000006|0x00000005|02_MIDDLE|Cargo manifests+lock|map library resolution boundary|OBSERVED|crate source/build-script closure not fully vendored/captured|close first forensic unit without claiming exhaustive completion
