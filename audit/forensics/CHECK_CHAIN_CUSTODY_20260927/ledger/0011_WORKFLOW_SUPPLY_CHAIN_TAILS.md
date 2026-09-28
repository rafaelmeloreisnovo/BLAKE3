# μ0011 — Workflow supply-chain tails

- date: 2026-09-27
- parent_commit: 35b9fef11066c47744accde125ed9919f8614e07
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: WORKFLOW_SUPPLY_CHAIN
- claim_allowed: false

## External GitHub Actions observed

Mutable major/channel refs:
- `actions/checkout@v4`
- `actions/checkout@v6`
- `actions/setup-python@v4`
- `actions/setup-python@v6`
- `actions/upload-artifact@v4`
- `actions/download-artifact@v4`
- `dtolnay/rust-toolchain@master`
- `dtolnay/rust-toolchain@stable`
- `dtolnay/rust-toolchain@nightly`
- `dtolnay/rust-toolchain@1.85.1`
- `addnab/docker-run-action@v3`

Pinned commit observed:
- `lukka/get-cmake@5f6e04f5267c8133f1273bf2103583fc72c46b17`

A tag/channel reference can resolve to different action code at different times. A full commit SHA materially reduces that mutable-reference class.

## Remote installer tails in upstream CI

The base `ci.yml` includes direct remote-script execution:

`curl https://wasmtime.dev/install.sh -sSf | bash`

and, inside the GCC 5.4 container path:

`curl https://sh.rustup.rs -sSf | sh -s -- -y --profile minimal`

These are explicit in the base YAML but execute content fetched at runtime. The fetched script body is not represented by the audited repository tree.

Classification:
`REMOTE_SCRIPT_BODY = EXTERNAL_RUNTIME_INPUT`

## Package-manager resolution tails

Observed commands include:
- `cargo install cross` without explicit version;
- `pip install PyGithub` without explicit version;
- multiple `apt-get install` commands;
- `brew install tbb`;
- `rustup target add wasm32-unknown-unknown`.

These commands resolve artifacts through external package repositories at execution time. Exact package bytes are not fixed by the Git commit alone unless the external repository snapshot/version/digest is separately captured.

## Container tail

Observed:

`docker run ... messense/cargo-xwin cargo xwin test ...`

No digest is present in the command.

Also observed:
`addnab/docker-run-action@v3`.

Therefore both action implementation and container image resolution introduce external supply-chain edges.

## Runner-image shadow

Jobs use mutable hosted image labels such as:
- `ubuntu-latest`
- `macOS-latest`
- matrix entries including `windows-latest`, `ubuntu-latest`, `macOS-latest`.

A Git commit does not encode the exact OS image package set behind a `*-latest` label.

## Why this matters to “commit causes activity”

The actual execution set is larger than the textual commands stored in the commit:

`commit -> GitHub runner image -> external action ref -> shell command -> package manager / remote installer / container -> resolved artifact`

Each arrow may introduce code that is not stored in the repository itself.

## Obfuscation classification

Observed:
- transitive/indirect execution: YES
- mutable external references: YES
- remote runtime code fetch: YES
- encrypted/encoded hidden command payload: NOT_OBSERVED
- evidence of malicious intent: NOT_ESTABLISHED

The correct term for the observed phenomenon is supply-chain indirection / runtime resolution, not cryptographic command concealment.

## R3

- F_ok: external Actions, runner labels, installers, package managers, and container tails mapped.
- F_gap: RMR shell import graph and external Git fetch graph still need bottom-up expansion.
- F_next: map every `source`, script-to-script call, git clone/fetch, and command-substitution edge reachable from RMR workflows.
