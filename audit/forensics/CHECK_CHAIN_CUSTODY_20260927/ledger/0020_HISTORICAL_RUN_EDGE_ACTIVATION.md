# μ0020 — Historical run activation: possible edge vs executed edge

- date: 2026-09-27
- parent_commit: `8eacfa0dca036b204a75cdc74bb43ab27fda4b06`
- audited_commit: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- base_ci_run: `36287272113`
- rmr_comprehensive_run: `36287272177`
- kind: HISTORICAL_EXECUTION_EDGE_CORRELATION
- completion_state: PARTIAL_PAGE_BOUNDED
- claim_allowed: false

## μ0_START — correlation contract

Source inspection establishes capabilities. This grain requires historical execution evidence before promoting a capability to an executed edge.

States used:

- `POSSIBLE_EDGE`: source permits the route, no run proof here;
- `EXECUTED_STEP`: GitHub job/step records show the step completed;
- `EXECUTED_INNER_EFFECT`: job log shows the inner download/process/materialization;
- `TOKEN_VAZIO_INNER_EFFECT`: outer step ran but exact inner bytes/effect were not captured;
- `NOT_EXECUTED`: explicit skip/condition evidence.

Boundary:

`POSSIBLE_EDGE != EXECUTED_STEP != EXECUTED_INNER_EFFECT`

## μ1_RESOLVE — base CI run 36287272113

### A. pagination boundary

The available workflow-jobs wrapper returned exactly 30 jobs for this run and documents itself as first-page only.

The same run is known from the Checks API audit to have 74 CI check-runs.

Therefore this grain does not claim exhaustive job enumeration.

`RETURNED_JOBS=30`

`COMPLETE_JOB_SET=TOKEN_VAZIO_PAGINATION`

### B. mutable Rust toolchain action executed

Multiple returned jobs completed steps using `dtolnay/rust-toolchain@master`, `@stable`, or a version label.

For the WASM job `108530284367`, the runtime log resolved:

- `actions/checkout@v4` -> action SHA `11d5960a326750d5838078e36cf38b85af677262`;
- `dtolnay/rust-toolchain@stable` -> action SHA `6bed0761d98439e5a578e2877258200ad565ba87`.

The action log itself executed:

`curl --proto '=https' ... https://sh.rustup.rs | sh -s -- --default-toolchain none -y`

Classification:
- action use: EXECUTED_STEP;
- resolved action SHA for this job: OBSERVED;
- rustup remote installer invocation: EXECUTED_INNER_EFFECT;
- exact downloaded installer bytes/hash: TOKEN_VAZIO.

### C. Wasmtime remote installer executed

Job `108530284367` — `WASM tests` — concluded success.

Step `install Wasmtime` concluded success.

Log proves execution of:

`curl https://wasmtime.dev/install.sh -sSf | bash`

The installer selected `Wasmtime v49.0.1` and fetched the release archive from the Bytecode Alliance GitHub release path for `wasmtime-v49.0.1-x86_64-linux.tar.xz`.

Subsequent `cargo test --target wasm32-wasip1` steps succeeded.

Classification:
- remote script pipe-to-shell: EXECUTED_INNER_EFFECT;
- Wasmtime version: OBSERVED `49.0.1`;
- archive URL identity: OBSERVED;
- archive digest/signature: TOKEN_VAZIO in captured log;
- installer script digest: TOKEN_VAZIO.

### D. root Cargo resolution changed beyond committed b3sum lock snapshot

The same WASM job log shows runtime downloads including, among others:

- `cc v1.5.1`;
- `cpufeatures v0.3.1`;
- `cfg-if v1.0.5`;
- `serde v1.0.229`;
- `serde_derive v1.0.229`;
- `syn v3.0.6`;
- `proc-macro2 v1.0.107`;
- `quote v1.0.47`.

This is direct historical evidence that the root build resolved package versions at run time.

It also demonstrates why `Cargo.toml version requirement != immutable dependency bytes`.

### E. cargo install cross executed

Job `108530284307` — `cross powerpc64-unknown-linux-gnu` — concluded success.

Log proves:

- `cargo install cross` executed;
- Cargo downloaded `cross v0.2.5`;
- Cargo installed `cross v0.2.5`;
- many transitive crates were downloaded before execution of cross-target commands.

Classification:
- unversioned source command: EXECUTED_STEP;
- actual selected cross version: OBSERVED `0.2.5`;
- package/transitive fetch: EXECUTED_INNER_EFFECT;
- complete byte-level package custody: TOKEN_VAZIO.

### F. mutable Docker tag executed and digest recovered

Job `108530284334` — `cargo xwin test` — concluded success.

Log proves:

`docker run ... messense/cargo-xwin ...`

Docker reported:

- local image absent;
- `messense/cargo-xwin:latest` pulled from registry;
- layers downloaded;
- resulting image digest:
  `sha256:9856b895265d4966f212228ba64802cf89337e2a2a537aa2533c1b8784cbc81b`.

The container then downloaded an MSVC CRT and Cargo packages.

Classification:
- mutable tag `:latest`: EXECUTED_STEP;
- content digest for this historical run: OBSERVED;
- MSVC CRT download: EXECUTED_INNER_EFFECT;
- CRT artifact digest/source receipt: TOKEN_VAZIO in captured log.

This is a key distinction:

`MUTABLE_TAG_AT_SOURCE != UNRECOVERABLE_HISTORICAL_IMAGE`

For this run the image digest is recoverable from logs even though the YAML did not pin it.

### G. TBB execution

Returned Linux CMake jobs with `BLAKE3_USE_TBB=ON` succeeded.

Job log shows Ubuntu packages such as `libtbb-dev 2021.11.0-2ubuntu2` installed and CMake compiling `blake3_tbb.cpp`.

Thus the TBB path was executed in the returned Linux job.

This does not prove that the optional CMake `FetchContent(oneTBB)` route ran in these Linux jobs; package-manager TBB satisfied the dependency.

Windows matrix jobs that may enable `BLAKE3_FETCH_TBB=YES` are outside the 30-job page returned here.

Classification:
- TBB library path: EXECUTED_INNER_EFFECT;
- Linux FetchContent route: NOT_PROVEN / apparently not required in observed job;
- Windows FetchContent route in this run: TOKEN_VAZIO_PAGINATION.

### H. C bindings build.rs executed

Returned Rust jobs show successful steps such as:

- `cargo test C bindings assembly`;
- `cargo test C bindings intrinsics`;
- `cargo test C bindings no AVX-512`;
- `cargo test C bindings no AVX2`;
- `cargo test C bindings no SSE41`;
- `cargo test C bindings no SSE2`.

Therefore the C-bindings Cargo package and its build-script/native compilation route were activated.

However no captured evidence shows `BLAKE3_C_DIR_OVERRIDE` was set.

Classification:
- build.rs route: EXECUTED_STEP;
- alternate source-root override: POSSIBLE_EDGE;
- override activation: TOKEN_VAZIO/NOT_OBSERVED.

## μ2_EXECUTE_OBSERVE — effective token contrast

### Base CI job

Job `108530284367` log recorded the effective `GITHUB_TOKEN Permissions` block:

- Actions: write
- ArtifactMetadata: write
- Attestations: write
- Checks: write
- CodeQuality: write
- Contents: write
- CopilotRequests: write
- Deployments: write
- Discussions: write
- Drives: write
- Issues: write
- Metadata: read
- Models: read
- Packages: write
- Pages: write
- PullRequests: write
- RepositoryProjects: write
- SecurityEvents: write
- Statuses: write
- VulnerabilityAlerts: read

It also recorded:

- `Secret source: Actions`;
- `actions/checkout@v4` with `persist-credentials: true`;
- repository auth installed into Git config as a masked HTTP extraheader.

This is stronger than the source-only observation from μ0019:

`BASE_CI_EFFECTIVE_TOKEN = BROAD_WRITE` for this observed job/run.

No evidence in this grain proves that later commands actually used that write authority to mutate the repository.

But remote installer/package/container code executed in the same job context while the checkout credential was persisted.

Classification:

`BROAD_WRITE_CREDENTIAL_PRESENT = OBSERVED`

`REMOTE_CODE_EXECUTION = OBSERVED`

`REMOTE_CODE_USED_GITHUB_CREDENTIAL = NOT_PROVEN`

### RMR comprehensive comparison

Run `36287272177`, source job `108539902485`, logged:

- `Contents: read`;
- `Metadata: read`;
- no broad write permissions in the displayed token block;
- `actions/checkout@v6` with `persist-credentials: false`;
- exact checkout ref `b1c8cab1d75943a628e447fa72bb06ebeb348489`;
- `OFFICIAL_REF=6aab490a26124663329dfd3961b8469f8fdb158b`;
- resolved checkout action SHA `d23441a48e516b6c34aea4fa41551a30e30af803`;
- resolved upload-artifact action SHA `ea165f8d65b6e75b540449e92b4886f43607fa02`.

This validates that the RMR workflow hardening changed actual runtime authority, not merely YAML appearance.

## μ3_CLOSE — reconstruction

The historical run proves the following execution chain existed on the audited master:

`push -> base ci.yml -> broad-write GITHUB_TOKEN -> checkout credential persisted`

and, in different jobs of the same workflow:

- `rust-toolchain action -> remote rustup installer`;
- `curl wasmtime install.sh | bash -> Wasmtime 49.0.1`;
- `cargo install cross -> cross 0.2.5 + transitive crates`;
- `docker :latest -> pulled image -> recovered digest 9856...cbc81b -> MSVC CRT download`;
- `root cargo build/test -> run-time dependency resolution`;
- `C bindings tests -> build.rs/native C/ASM route`;
- `CMake TBB ON -> system TBB package -> blake3_tbb.cpp compilation`.

No encrypted command stream or deliberate obfuscation was established.

The material issue is transitive authority and mutable external execution combined with a broad-write job credential.

### R3

- F_ok: multiple high-impact source edges promoted from POSSIBLE to historically EXECUTED with job/log evidence.
- F_gap: job enumeration is page-limited; exact hashes for remote installer bodies, downloaded crates in the root resolver, Wasmtime archive and MSVC CRT remain incomplete.
- F_next: construct a bottom-up executable/library/function boundary map and identify which external code can see or inherit credentials/environment.
