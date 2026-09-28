# μTRACE 0x0000000A — OBSERVE / third-party action descriptors

parent_mu=0x00000009
phase=OBSERVE
function=CONTROL_GRAFO
procedure_id=ACTDESC-0001
timestamp_local=2026-09-27T21:31-03:00
evidence_state=OBSERVED
claim_allowed=false

## actions/checkout @ 11d5960...
runtime=node20
main=dist/index.js
post=dist/index.js

High-impact declared inputs:
- token defaults to github.token
- persist-credentials defaults true; token/SSH key may be configured into local git config for subsequent authenticated git commands; post step removes it
- clean defaults true -> git clean/reset behavior
- fetch-depth defaults 1
- submodules defaults false
- set-safe-directory defaults true -> modifies global Git safe.directory
- github-server-url can redirect GitHub instance target
- allow-unsafe-pr-checkout exists, default false

Finding: a single `uses: actions/checkout@v4` line expands into credential configuration, Git configuration, fetch/clean behavior and post-job cleanup. This is OBSERVED indirection, not proof of concealment.

## dtolnay/rust-toolchain @ 02cb101...
runtime=composite/bash
Observed:
- parses toolchain inputs dynamically
- sets CARGO_HOME/GITHUB_PATH
- if rustup absent: curl sh.rustup.rs | sh on non-Windows
- Windows downloads rustup-init.exe and executes it
- executes rustup toolchain install/default
- mutates CARGO_INCREMENTAL, CARGO_TERM_COLOR
- conditionally changes Cargo crates.io protocol
- conditionally changes CARGO_HTTP_MULTIPLEXING

This establishes a direct action -> network installer -> Rust toolchain -> Cargo environment route.

## addnab/docker-run-action @ 4f65fab...
runtime=docker
image=Dockerfile
Inputs directly control image/options/run/shell/registry/credentials/network.
The action descriptor itself delegates behavior to its Dockerfile/entrypoint; that is the next transitive edge.

## lukka/get-cmake @ 5f6e04...
runtime=node20
main=dist/index.js
Inputs include cmakeVersion/ninjaVersion; `latest` is explicitly supported and is default when unspecified. Cloud/local caches are configurable.
The repository workflow passes `latest` in a matrix for current CMake CI.

## Classification
ACTION_INPUT_CONTROL_SURFACE=OBSERVED
CHECKOUT_CREDENTIAL_PERSISTENCE_CAPABILITY=OBSERVED_DEFAULT_TRUE
RUST_TOOLCHAIN_NETWORK_INSTALL_EDGE=OBSERVED
RUST_CARGO_ENVIRONMENT_MUTATION=OBSERVED
DOCKER_INPUT_TO_EXECUTION_SURFACE=OBSERVED
LATEST_TOOL_RESOLUTION=OBSERVED
BUNDLED_NODE_DIST_INTERNALS=TOKEN_VAZIO_PENDING_READ
OBFUSCATED_INTENT=NOT_PROVEN

syslog=OBSERVE|0x0000000A|ACTDESC-0001|descriptor_expansion_complete|next=pre_intent_docker_and_node_dist
