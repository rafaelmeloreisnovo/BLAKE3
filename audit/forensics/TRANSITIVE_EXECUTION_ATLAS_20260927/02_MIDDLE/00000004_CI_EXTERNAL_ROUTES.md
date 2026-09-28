# μTRACE 0x00000004 — MIDDLE / upstream CI external execution routes

parent_mu=0x00000003
phase=02_MIDDLE
source_ref=.github/workflows/ci.yml@5365b389919881f91be4a407ab5b2d094bc1a65c
evidence_state=OBSERVED
claim_allowed=false

## Trigger
push on every branch ("*") and pull_request.

## External action routes
- actions/checkout@v4 — tag/major ref, not immutable commit SHA.
- dtolnay/rust-toolchain@master — mutable branch ref.
- dtolnay/rust-toolchain@stable — moving channel/ref.
- dtolnay/rust-toolchain@nightly — moving channel/ref.
- dtolnay/rust-toolchain@1.85.1 — version-like ref, not proven immutable by this record.
- addnab/docker-run-action@v3 — major tag ref.
- lukka/get-cmake@5f6e04f5267c8133f1273bf2103583fc72c46b17 — commit-SHA pinned.

## Direct network/package/runtime routes
- cargo install cross — package acquisition without explicit version in command.
- curl https://wasmtime.dev/install.sh -sSf | bash — downloaded script piped directly to shell.
- docker run messense/cargo-xwin ... — image reference without digest.
- apt-get update/install — mutable repository/package state.
- brew update/install — mutable repository/package state.
- curl https://sh.rustup.rs -sSf | sh ... — downloaded script piped directly to shell.
- cmakeVersion/latest and ninjaVersion/latest — moving tool selection.
- CMake on Windows enables BLAKE3_FETCH_TBB=YES, activating SHA-pinned oneTBB FetchContent if TBB absent.

## Human-visible parent vs transitive effect
A commit/push can therefore cause execution/acquisition not encoded in that commit's source blobs alone. This is a supply-chain/reproducibility property.

## Classification
REMOTE_SCRIPT_PIPE_TO_SHELL=OBSERVED
UNPINNED_CONTAINER_DIGEST=OBSERVED
MOVING_ACTION_OR_TOOL_REFS=OBSERVED
PACKAGE_MANAGER_MUTABLE_STATE=OBSERVED
SHA_PINNED_ACTION=OBSERVED
CRYPTOGRAPHIC_COMMAND_OBFUSCATION=NOT_PROVEN
MALICIOUS_CONTROL=NOT_PROVEN

mu_record=0x00000004|0x00000003|02_MIDDLE|ci.yml|map external execution routes|OBSERVED|transitive contents of remote actions/packages not yet captured|map RMR script-source/network graph
