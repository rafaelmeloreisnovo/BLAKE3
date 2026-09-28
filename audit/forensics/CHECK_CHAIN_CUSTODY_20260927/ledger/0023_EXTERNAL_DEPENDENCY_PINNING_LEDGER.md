# μ0023 — External dependency ledger by pinning and execution class

- date: 2026-09-27
- parent_commit: `8a360aa579783de7eb6c8b7abf83d46a401f495e`
- audited_tree: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- kind: EXTERNAL_DEPENDENCY_PINNING_LEDGER
- completion_state: SOURCE_ENUMERATED_RUNTIME_PARTIAL
- claim_allowed: false

## μ0_START — classification

Every external workflow edge is assigned one of these source classes:

- `PINNED_SHA`: exact immutable Git commit in workflow/source;
- `MUTABLE_TAG`: action/container ref such as v4/v6/v3/latest/stable/master/nightly;
- `MUTABLE_VERSION_SELECTOR`: source asks resolver for latest/semver-compatible/current package;
- `SYSTEM_PACKAGE`: apt/brew package resolved from runner package repositories;
- `REMOTE_SCRIPT`: network response is directly interpreted/executed;
- `PINNED_GIT_CHILD`: child repository fetched at an exact Git commit;
- `LOCAL_ONLY`: no external fetch at that edge.

This ledger describes source authority. Historical run receipts may promote a mutable source ref to an observed exact SHA/digest for one run.

## μ1_RESOLVE — workflow external atlas

### A. inherited/base ci.yml

GitHub Actions:
- `actions/checkout@v4` — MUTABLE_TAG;
- `dtolnay/rust-toolchain@master` — MUTABLE_TAG;
- `dtolnay/rust-toolchain@stable` — MUTABLE_TAG;
- `dtolnay/rust-toolchain@nightly` — MUTABLE_TAG;
- `dtolnay/rust-toolchain@1.85.1` — version tag, not full commit;
- `addnab/docker-run-action@v3` — MUTABLE_TAG;
- `lukka/get-cmake@5f6e04f5267c8133f1273bf2103583fc72c46b17` — PINNED_SHA.

Direct package/install/fetch edges:
- `cargo install cross` — MUTABLE_VERSION_SELECTOR; observed historical resolution cross 0.2.5;
- `curl https://wasmtime.dev/install.sh | bash` — REMOTE_SCRIPT;
- `docker run messense/cargo-xwin` — implicit `:latest`, MUTABLE_TAG; historical digest recovered;
- apt packages: ninja-build, TBB, libc++, build tools, tcc — SYSTEM_PACKAGE;
- brew TBB — SYSTEM_PACKAGE;
- inner `curl https://sh.rustup.rs | sh` — REMOTE_SCRIPT;
- CMake/Ninja inputs `latest` to get-cmake — MUTABLE_VERSION_SELECTOR.

CMake child:
- oneTBB `0c0ff192a2304e114bc9e6557582dfba101360ff` — PINNED_GIT_CHILD.

### B. tag.yml

GitHub Actions:
- `actions/checkout@v4` — MUTABLE_TAG;
- `actions/setup-python@v4` — MUTABLE_TAG;
- `dtolnay/rust-toolchain@stable` — MUTABLE_TAG.

Packages:
- `pip install PyGithub` — MUTABLE_VERSION_SELECTOR;
- `apt-get install musl-tools` — SYSTEM_PACKAGE.

Authority:
- release helper receives `secrets.GITHUB_TOKEN`;
- exact effective token permissions depend on run/repository policy unless recovered from a historical tag job log.

### C. RMR workflow family

Observed action references:
- `actions/checkout@v6` in many newer RMR workflows — MUTABLE_TAG;
- `actions/checkout@v4` in selected RMR boundary/freestanding/standalone workflows — MUTABLE_TAG;
- `actions/upload-artifact@v4` — MUTABLE_TAG;
- `actions/download-artifact@v4` — MUTABLE_TAG;
- `actions/setup-python@v6` — MUTABLE_TAG.

System packages used across RMR workflows include:
- clang;
- cmake;
- ninja-build;
- build-essential;
- lld;
- binutils;
- pkg-config;
- libtbb-dev;
- libssl-dev.

These are SYSTEM_PACKAGE edges unless the runner repository snapshot/package version is captured separately.

`rustup target add wasm32-unknown-unknown` is a network/toolchain component resolution edge whose exact component artifact is not pinned by the workflow line alone.

### D. RMR upstream comparison children

RMR V3 source scripts use:
- official repository URL as default;
- exact `OFFICIAL_REF=6aab490a26124663329dfd3961b8469f8fdb158b` in the comprehensive workflow/source contract.

Class:
`PINNED_GIT_CHILD` for the audited comprehensive route.

Other local benchmark helper scripts documented in earlier grains can clone default upstream HEAD without a supplied ref; those remain mutable auxiliary routes outside the pinned comprehensive V3 path.

## μ2_EXECUTE_OBSERVE — source pin vs run resolution

Historical run `36287272113` demonstrates why both fields are needed.

Examples:

| Source ref | Source class | Historical resolution |
| --- | --- | --- |
| actions/checkout@v4 | MUTABLE_TAG | `11d5960a326750d5838078e36cf38b85af677262` in sampled jobs |
| dtolnay/rust-toolchain@stable | MUTABLE_TAG | `6bed0761d98439e5a578e2877258200ad565ba87` in WASM job |
| addnab/docker-run-action@v3 | MUTABLE_TAG | `4f65fabd2431ebc8d299f8e5a018d79a769ae185` |
| messense/cargo-xwin:latest | MUTABLE_TAG | `sha256:9856b895265d4966f212228ba64802cf89337e2a2a537aa2533c1b8784cbc81b` |
| gcc:5.4 | mutable image tag | `sha256:e6ef7f0295b9d915f8521de360e30803bf8561cfb9cea8e320aa66761be8ec42` |
| docker:20.10 action base | mutable image tag in action | `sha256:2967f0819c84dd589ed0a023b9d25dcfe7a3c123d5bf784ffbb77edf55335f0c` |
| cargo install cross | MUTABLE_VERSION_SELECTOR | cross 0.2.5 |
| Wasmtime installer | REMOTE_SCRIPT | selected Wasmtime 49.0.1; installer/archive digest missing |
| get-cmake action | PINNED_SHA | same pinned SHA |
| cmakeVersion latest | MUTABLE_VERSION_SELECTOR | CMake 3.31.5 in sampled Windows job |
| oneTBB Git child | PINNED_GIT_CHILD | source declares 0c0ff192...; fetched build path observed |

For RMR comprehensive run `36287272177`:

- `actions/checkout@v6` resolved to `d23441a48e516b6c34aea4fa41551a30e30af803` in sampled source job;
- `actions/upload-artifact@v4` resolved to `ea165f8d65b6e75b540449e92b4886f43607fa02`.

Therefore mutable major tags can be reconstructed per historical run when the Actions log retains the resolved SHA.

## μ3_CLOSE — governance route

### High-priority mutable/remote edges

Priority A — remote code directly interpreted/executed:
1. rustup shell installer;
2. Wasmtime shell installer.

Priority B — external executable environment:
3. container images by mutable tags;
4. GitHub Actions by mutable major/channel refs;
5. `cargo install cross` without exact version/lock;
6. PyGithub without exact version/hash.

Priority C — system/tool resolvers:
7. apt packages;
8. brew packages;
9. get-cmake `latest` tool selector;
10. rustup target component resolution;
11. root Cargo dependency resolution without committed root lock.

Priority D — pinned child but byte receipt incomplete:
12. oneTBB exact commit fetch;
13. official BLAKE3 exact commit fetch.

### Required immutable receipt tuple

For every external edge:

`<parent_step, resolver, requested_ref, resolved_ref, content_hash, origin, privilege_context, environment_subset, result>`

If any element required for the claim is unavailable:

`TOKEN_VAZIO`

rather than inferred PASS.

### R3

- F_ok: external workflow dependency classes enumerated and connected to known historical resolutions.
- F_gap: package/archive/script byte hashes remain incomplete and action tags are not source-pinned.
- F_next: build the canonical route atlas linking commit -> workflow -> job -> action/script -> dependency -> function/backend -> artifact/evidence, with explicit missing-evidence nodes.
