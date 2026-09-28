# μ0016 — Bottom-up forensic checkpoint

- date: 2026-09-27
- parent_commit: 414e42bb51a8f85a544ae2daee6078151789b7d0
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: FORENSIC_CHECKPOINT
- completion_state: PARTIAL
- claim_allowed: false

## Materialized layers

μ0009:
- complete tree census and executable/build/config surface.

μ0010:
- Cargo manifests, lock asymmetry, build.rs, cc/native compiler and Wasmtime runner shadows.

μ0011:
- GitHub Actions, mutable refs, remote installers, package managers, containers and runner-image tails.

μ0012:
- RMR shell imports, common.sh, upstream Git clone/fetch, environment override surfaces and compiler fan-out.

μ0013:
- path-level authorship/committer provenance separated from later runtime causality.

μ0014:
- CMake child-directory chain and conditional oneTBB FetchContent pinned to Git SHA.

μ0015:
- OpenSSL::Crypto and provider-backed runtime implementation-selection shadow.

## Current transitive model

`commit/push`
-> GitHub event
-> workflow
-> hosted runner image
-> external Action
-> shell command
-> local script
-> sourced shell library
-> environment selectors
-> Git/Cargo/CMake/package manager/container
-> build.rs / generated build commands
-> compiler/assembler/linker
-> linked library
-> runtime provider/runner
-> test/report/audit gate
-> check conclusion

Not every edge is active in every run. Activation depends on workflow, path filters, matrices, feature flags, environment, platform and dependency availability.

## Confirmed external/runtime-resolution classes

- GitHub Action refs not uniformly pinned to immutable SHAs;
- hosted `*-latest` images;
- remote `curl | bash` installers;
- package-manager resolution;
- container image without digest in base CI;
- Cargo root resolution without committed root lock;
- upstream Git clone/fetch in RMR comparison tooling;
- CMake oneTBB FetchContent under enabling conditions;
- system OpenSSL discovery;
- OpenSSL provider implementation selection;
- Wasmtime runner indirection.

## Negative findings retained

- no Git submodule observed in audited tree;
- no versioned Git hook observed;
- no custom .github/actions directory observed;
- no executable symlink chain observed;
- no encrypted/encoded command decoder observed;
- no evidence sufficient to classify deliberate concealment;
- no evidence sufficient to identify an unauthorized runtime controller.

## Remaining forensic work

Status remains PARTIAL.

Still to expand grain-by-grain:
1. all shell command substitutions and pipelines, classifying execution vs data transformation;
2. all Python imports/subprocess/process-launch edges;
3. all Rust build/test helper crates and procedural-macro execution surfaces;
4. all CMake generated commands and platform branches;
5. exact shared-library/ELF dependencies of produced artifacts where receipts exist;
6. all environment variables crossing workflow -> script -> build -> runtime;
7. historical introduction/change commits for each high-impact edge;
8. CI run correlation: which edges were actually activated per historical run;
9. immutable hashes/digests for external artifacts where retrievable;
10. final transitive execution DAG and gap matrix.

## Evidence discipline

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`

`POSSIBLE_EDGE != EXECUTED_EDGE`

`AUTHORSHIP != RUNTIME_CONTROL`

`INDIRECTION != OBFUSCATION`

Any missing historical environment, provider binary, package byte set or remote installer body remains `TOKEN_VAZIO` unless independently recovered.

## R3

- F_ok: first seven bottom-up layers are append-only and independently reviewable.
- F_gap: process-launch/import/environment and per-run activation graph remain incomplete.
- F_next: continue with shell/Python process graph, then correlate each edge to historical workflow runs.
