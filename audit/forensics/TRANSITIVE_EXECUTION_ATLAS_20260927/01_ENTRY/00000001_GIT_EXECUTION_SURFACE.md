# μTRACE 0x00000001 — ENTRY / Git execution surface inventory

parent_mu=0x00000000
phase=01_ENTRY
source_ref=git-tree@5365b389919881f91be4a407ab5b2d094bc1a65c
evidence_state=OBSERVED
tree_complete=true
tree_entries=1086
source_mutation=false
claim_allowed=false

## Direct observations
- executable mode 100755: 29 paths
- symlink mode 120000: 3 paths, all b3sum license links
- gitlink/submodule mode 160000: 0
- workflow YAML files: 14
- identified build-control surfaces: 16

### Executable paths
.github/workflows/upload_github_release_asset.py
c/blake3_c_rust_bindings/cross_test.sh
c/test.py
rmr/build/build_omega.sh
rmr/crypto_freestanding140/audit/audit_artifact.sh
rmr/crypto_freestanding140/audit/audit_source_shape.sh
rmr/crypto_freestanding140/build/build_cross_matrix.sh
rmr/crypto_freestanding140/build/build_host_selftest.sh
rmr/freestanding_custody16/audit/audit_artifact.sh
rmr/freestanding_custody16/build/build_cross_matrix.sh
rmr/freestanding_custody16/build/build_freestanding_probe.sh
rmr/freestanding_custody16/build/build_host_selftest.sh
rmr/freestanding_custody16/build/build_termux_snapshot.sh
rmr/standalone/audit/audit_artifact.sh
rmr/standalone/build/build_cross_matrix.sh
rmr/standalone/build/build_host_selftest.sh
rmr/tests/test_pipeline.sh
rmr/tools/audit_hash_boundaries.sh
rmr/tools/audit_pathcutter_static.py
rmr/tools/audit_pure_core_boundaries.sh
rmr/tools/build_pai.sh
rmr/tools/orchestrate_blake3_compare.sh
rmr/tools/run_full_audit.sh
test_vectors/cross_test.sh
tools/benchmark_compare_official.sh
tools/benchmark_example.sh
tools/check_rmr_headers.py
tools/check_rmr_headers.sh
tools/cmake_ci_compare.sh

### Build-control surfaces
.cargo/config.toml
Cargo.toml
b3sum/Cargo.lock
b3sum/Cargo.toml
build.rs
c/CMakeLists.txt
c/blake3_c_rust_bindings/Cargo.toml
c/blake3_c_rust_bindings/build.rs
c/dependencies/CMakeLists.txt
c/dependencies/tbb/CMakeLists.txt
reference_impl/Cargo.toml
rmr/CMakeLists.txt
test_vectors/Cargo.toml
tools/compiler_version/Cargo.toml
tools/compiler_version/build.rs
tools/instruction_set_support/Cargo.toml

### Workflow surface
14 workflow YAML files under .github/workflows.

## Classification
SUBMODULE_EXECUTION_EDGE=NOT_OBSERVED
VERSIONED_GITLINK=NOT_OBSERVED
SYMLINK_EXECUTION_EDGE=NOT_OBSERVED
EXECUTABLE_SURFACE=OBSERVED
BUILD_INDIRECTION_SURFACE=OBSERVED

mu_record=0x00000001|0x00000000|01_ENTRY|git-tree|inventory modes/surfaces|OBSERVED|transitive contents not exhausted|inspect workflow/build dependency edges
