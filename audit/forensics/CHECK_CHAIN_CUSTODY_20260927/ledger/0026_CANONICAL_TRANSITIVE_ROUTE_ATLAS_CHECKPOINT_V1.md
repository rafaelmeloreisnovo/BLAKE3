# μ0026 — Canonical transitive route atlas checkpoint V1

- date: 2026-09-27
- parent_commit: `31f754e3e261cc99631bc86279b22974be78ff78`
- audited_fork_commit: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- kind: TRANSITIVE_EXECUTION_ROUTE_ATLAS
- checkpoint: true
- final: false
- claim_allowed: false

## μ0_START — atlas contract

This checkpoint joins the granular ledgers into stable route identifiers.

Each route records:

`ROUTE_ID | parent | child | state | authority | evidence | gap`

States:
- `EXECUTED`: historical run/artifact proves activation;
- `POSSIBLE`: source permits path but activation not proven;
- `NOT_OBSERVED`: searched/inspected and not found in audited scope;
- `TOKEN_VAZIO`: required evidence unavailable;
- `CONDITIONAL`: activation depends on target/feature/event/configuration.

The atlas is append-only. Future findings extend or supersede states by new receipt; they do not erase prior observations.

## μ1_RESOLVE — canonical routes

### R001 — branch push to inherited CI

`push(any branch) -> .github/workflows/ci.yml`

State: `EXECUTED`.

Evidence:
- workflow source trigger;
- run `36287272113`.

Authority:
- historical base-CI job token carried broad write permissions.

Gap:
- repository-wide default permission policy as a settings object was not directly exported, but job runtime permissions are observed.

---

### R002 — checkout credential persistence

`base ci job -> actions/checkout@v4 -> persisted Git credential`

State: `EXECUTED`.

Historical action SHA:
`11d5960a326750d5838078e36cf38b85af677262`.

Observed:
- `persist-credentials: true`;
- masked Git HTTP extraheader installed;
- broad-write token context.

Risk boundary:
external code running later in the job may share the same workspace.

Use of the credential by external child code: `NOT_PROVEN`.

---

### R003 — Rust toolchain action to remote rustup script

`ci.yml -> dtolnay/rust-toolchain@stable/master/etc -> curl sh.rustup.rs | sh`

State: `EXECUTED` in sampled historical job.

Stable-job resolved action SHA:
`6bed0761d98439e5a578e2877258200ad565ba87`.

Remote installer body hash:
`TOKEN_VAZIO`.

---

### R004 — Wasmtime installer

`ci.yml -> curl wasmtime.dev/install.sh | bash -> Wasmtime -> wasm32-wasip1 test runner`

State: `EXECUTED`.

Historical selected version:
`Wasmtime 49.0.1`.

Installer/archive hashes:
`TOKEN_VAZIO`.

---

### R005 — Cargo install cross

`ci.yml -> cargo install cross -> registry resolution -> cross executable -> target tests`

State: `EXECUTED`.

Historical resolution:
`cross 0.2.5`.

Complete transitive crate-byte custody:
`TOKEN_VAZIO`.

---

### R006 — cargo-xwin container

`ci.yml -> docker run messense/cargo-xwin:latest -> container -> MSVC CRT/package downloads -> xwin test`

State: `EXECUTED`.

Historical image digest:
`sha256:9856b895265d4966f212228ba64802cf89337e2a2a537aa2533c1b8784cbc81b`.

MSVC CRT artifact hash:
`TOKEN_VAZIO`.

---

### R007 — GCC 5.4 container-action route

`ci.yml -> addnab/docker-run-action@v3 -> docker:20.10 action container -> gcc:5.4 container -> rustup remote script -> cargo test`

State: `EXECUTED`.

Resolved action SHA:
`4f65fabd2431ebc8d299f8e5a018d79a769ae185`.

Action-base image:
`sha256:2967f0819c84dd589ed0a023b9d25dcfe7a3c123d5bf784ffbb77edf55335f0c`.

GCC image:
`sha256:e6ef7f0295b9d915f8521de360e30803bf8561cfb9cea8e320aa66761be8ec42`.

Authority exposure:
- workspace mounted;
- Docker socket mounted;
- Actions runtime context injected;
- checkout credential persisted in workspace context.

External use of Git credential:
`NOT_PROVEN`.

---

### R008 — get-cmake latest selector

`ci.yml -> lukka/get-cmake@SHA -> cmakeVersion=latest -> installed CMake`

State: `EXECUTED`.

Action source:
`PINNED_SHA 5f6e04f5267c8133f1273bf2103583fc72c46b17`.

Historical selected CMake:
`3.31.5` in sampled Windows job.

Observation:
`ACTION_PINNED != TOOL_PINNED`.

---

### R009 — Windows oneTBB FetchContent

`Windows CMake TBB=ON -> BLAKE3_FETCH_TBB=YES -> FetchContent -> uxlfoundation/oneTBB -> compile/link tbb12.dll`

State: `EXECUTED`.

Declared child commit:
`0c0ff192a2304e114bc9e6557582dfba101360ff`.

Runtime evidence:
`_deps/tbb-src` compilation and TBB link/install.

Independent fetched-tree hash:
`TOKEN_VAZIO`.

---

### R010 — root Cargo unlocked resolution

`cargo build/test at repository root -> Cargo.toml semver requirements -> registry resolver -> selected crate versions`

State: `EXECUTED`.

Constraint:
no committed root `Cargo.lock`.

Historical root run showed versions such as:
- `cc 1.5.1`;
- `cpufeatures 0.3.1`;
- `cfg-if 1.0.5`;
- `serde 1.0.229`;
- `syn 3.0.6`.

Exact package-byte manifest per job:
`TOKEN_VAZIO`.

---

### R011 — root build.rs native compiler path

`Cargo -> build.rs -> cc crate -> C/ASM source selection -> native compiler -> linked Rust crate`

State: `EXECUTED`.

Selectors include:
- target;
- feature flags;
- `CC`;
- `CFLAGS`;
- compiler capability.

Exact child process graph varies by job/target.

---

### R012 — C bindings source override

`Cargo C-bindings -> build.rs -> BLAKE3_C_DIR_OVERRIDE -> alternate C/ASM source root`

State: `POSSIBLE`.

Capability:
`OBSERVED_IN_SOURCE`.

Historical activation:
`NOT_OBSERVED / TOKEN_VAZIO_ENV_RECEIPT`.

This is a source-authority boundary outside the Git tree if set.

---

### R013 — procedural macro compile-time execution

`b3sum -> clap derive -> clap_derive proc-macro -> proc-macro2/quote/syn -> compiler-time code execution`

State: `EXECUTED_OR_BUILD_REQUIRED` for derive-enabled b3sum builds; exact package-byte activation per historical invocation requires compiler build trace.

Source/resolution evidence:
b3sum manifest + lock graph.

Byte-level compiler plugin custody:
`PARTIAL`.

---

### R014 — BLAKE3 runtime CPU dispatch

`BLAKE3 API -> CPU feature detection -> AVX512/AVX2/SSE/NEON/portable backend`

State: `CONDITIONAL_EXECUTED`.

Control:
- CPU;
- target;
- compile definitions;
- Cargo/CMake feature gates.

Exact backend per individual hash call:
not retained in generic CI receipts.

---

### R015 — Rust FFI dispatch

`Rust Platform::detect -> cpufeatures -> unsafe extern C -> C/ASM symbol`

State: `CONDITIONAL_EXECUTED`.

Evidence:
source + successful backend tests.

Per-call backend telemetry:
`TOKEN_VAZIO`.

---

### R016 — serial vs Rayon topology

`Hasher/update route -> Join abstraction -> SerialJoin OR RayonJoin -> rayon_core::join`

State: `CONDITIONAL`.

Control:
Cargo `rayon` feature and API route.

Meaning:
logical digest equivalence does not imply identical scheduling/thread topology.

---

### R017 — official BLAKE3 comparison child

`RMR comprehensive workflow -> source/common.sh -> git fetch exact OFFICIAL_REF -> detached checkout -> comparative build/test`

State: `EXECUTED`.

Official commit:
`6aab490a26124663329dfd3961b8469f8fdb158b`.

RMR runtime authority:
- `Contents: read`;
- `Metadata: read`;
- checkout `persist-credentials: false`.

---

### R018 — RMR host crypto runtime

`RMR crypto API -> algorithm selector -> BLAKE3 backend OR OpenSSL EVP -> selftest`

State: `EXECUTED`.

Artifact evidence:
- RMR crypto objects compiled;
- static RMR crypto archive linked;
- runtime selftest passed.

Observed libssl-dev package:
`3.0.13-0ubuntu3.15`.

---

### R019 — OpenSSL provider resolution

`RMR EVP call -> EVP fetch/algorithm object -> OpenSSL provider resolution -> implementation`

State: `EXECUTED_WITH_PROVIDER_TOKEN_VAZIO`.

Functional execution:
supported by selftest.

Exact provider module:
`TOKEN_VAZIO_PROVIDER_TOOLCHAIN`.

No evidence of deliberate provider substitution.

---

### R020 — plain BLAKE3 ELF runtime

`BLAKE3 example PIE -> /lib64/ld-linux-x86-64.so.2 -> GLIBC imports -> executable code`

State: `EXECUTED_BUILD_ARTIFACT_OBSERVED`.

Observed:
- standard ELF interpreter;
- GLIBC versioned imports;
- PLT/GOT/dynamic structures;
- non-executable GNU stack.

Custom hidden loader:
`NOT_OBSERVED`.

---

### R021 — fork vs official binary-layout divergence

`same public ABI/symbol set -> different compiled .text/.eh_frame/static archive sizes`

State: `OBSERVED`.

Fork vs official:
- static archive: +160 bytes fork;
- `.text`: +128 bytes fork;
- `.eh_frame`: +48 bytes fork;
- section total: +176 bytes fork.

Cause:
`TOKEN_VAZIO_CAUSAL_ABLATION`.

---

### R022 — tag release mutation

`tag push -> tag.yml -> PyGithub -> GITHUB_TOKEN -> create/upload/delete release asset`

State: `POSSIBLE_BY_SOURCE`.

Trigger:
tag push only.

Historical tag-run activation in this atlas:
`TOKEN_VAZIO_NOT_CORRELATED_YET`.

Effective token permissions:
must be recovered from a tag run rather than inferred.

---

### R023 — Python local dynamic import

`test script -> importlib.spec_from_file_location -> repository-relative Python module -> module top-level execution`

State: `EXECUTED_WHEN_TEST_RUNS`.

Remote code load:
`NOT_OBSERVED`.

---

### R024 — Python executable/tool selection

`workflow/env/CLI -> Python -> shutil.which/subprocess argv -> compiler/readelf/nm/size/Cargo/local binary`

State: `EXECUTED` for observed tools.

Shell injection:
`NOT_OBSERVED` in inspected launchers.

---

### R025 — RMR HWIF architecture route

`CMake target processor -> x86_64/aarch64/armv7 source set -> optional privileged ARMv7 leaf -> static HWIF -> selftest`

State: `CONDITIONAL`.

Observed host selftest:
PASS.

Cross compile symbol matrix:
4/4 PASS in integration evidence.

Physical ARM execution:
`TOKEN_VAZIO`.

## μ2_EXECUTE_OBSERVE — atlas-level interpretation

### Confirmed high-impact composition

The most security-relevant observed base-CI composition is:

`push`
 -> broad-write token context
 -> checkout with persisted credentials
 -> external mutable-tag actions/tool resolvers
 -> remote scripts and containers
 -> workspace/toolchain/package execution.

This is an **authority + supply-chain co-location** finding.

It is not evidence that an external dependency actually exfiltrated, modified or abused credentials.

### Confirmed hardened contrast

RMR comprehensive V3 demonstrates:

`push/PR`
 -> explicit `contents: read`
 -> exact-head checkout
 -> `persist-credentials: false`
 -> exact official upstream commit
 -> retained artifacts/receipts.

This reduces authority exposure but does not make system packages, action tags or OpenSSL provider resolution fully hermetic.

### No deliberate obfuscation established

Across audited source searches and artifacts:

- no encoded/base64 decrypt-to-exec chain established;
- no repository `dlopen/dlsym` custom loader established;
- no Git submodule execution chain;
- no versioned Git hook chain;
- no arbitrary Python `eval/exec` execution chain;
- no malicious intent attribution.

What exists is a deep graph of legitimate but easy-to-miss indirections.

## μ3_CLOSE — checkpoint state

### F_ok

The atlas now binds:
- Git event;
- workflow;
- effective historical privilege;
- checkout credential behavior;
- external Actions;
- remote scripts;
- containers and observed digests;
- Cargo resolution/build scripts/proc macros;
- source override capability;
- CMake FetchContent;
- native CPU dispatch;
- Rust FFI;
- threading topology;
- ELF loader boundary;
- OpenSSL EVP/provider boundary;
- retained artifacts and hashes.

### F_gap

Still unresolved:
1. byte hashes for rustup/Wasmtime installer bodies and archives;
2. MSVC CRT artifact identity/hash;
3. complete crate-byte activation manifest for each Cargo job;
4. exact `BLAKE3_C_DIR_OVERRIDE` state in every relevant job;
5. provider module/libcrypto file hashes;
6. tag-release historical permission/execution correlation;
7. instruction-level causal diff for fork vs official binary divergence;
8. per-call SIMD/backend telemetry;
9. physical ARM execution for current RMR matrix;
10. external-code credential-access proof/denial.

### F_next

Proceed from atlas routes into atomic receipts:
- first: correlate tag/release history and token authority;
- second: derive exact external Actions SHAs/digests for every one of 74 base-CI jobs;
- third: inspect binary/link/provider artifacts where retained;
- fourth: perform causal ablation of the fork-vs-official machine-code delta without changing the audit branch source.
