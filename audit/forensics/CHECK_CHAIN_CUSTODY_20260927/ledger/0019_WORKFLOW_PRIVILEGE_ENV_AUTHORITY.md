# μ0019 — Workflow privilege, token and environment authority boundary

- date: 2026-09-27
- parent_commit: `52007ee26c9e929886bd2b72256ba17af0d02cc7`
- audited_tree: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- kind: WORKFLOW_PRIVILEGE_AND_ENVIRONMENT_GRAPH
- completion_state: PARTIAL
- claim_allowed: false

## μ0_START — authority questions

This grain asks:

1. which workflows can receive a token or secret?
2. which workflows explicitly constrain GitHub token permissions?
3. which event activates remote mutation?
4. which environment values cross from workflow into scripts/build systems?
5. which authority remains external to the audited commit?

Boundary:

`TOKEN_PRESENT != TOKEN_PERMISSION != TOKEN_USED != REMOTE_MUTATION`

## μ1_RESOLVE — observed privilege and environment edges

### A. RMR workflow permissions

The following audited RMR workflows explicitly declare:

`permissions:`
`  contents: read`

Observed paths:

- `rmr-blake3-repro.yml`;
- `rmr-blake3-upstream-compare-v2.yml`;
- `rmr-crypto-freestanding140-v1.yml`;
- `rmr-crypto-runtime.yml`;
- `rmr-full-validation.yml`;
- `rmr-hardware-build-topology.yml`;
- `rmr-hash-boundary-v1.yml`;
- `rmr-pure-core-boundary-v1.yml`;
- `rmr-simperf.yml`;
- `rmr-standalone-v1.yml`;
- `rmr-upstream-comprehensive-v3.yml`;
- `rmr-zip-custody.yml`.

This is an explicit read-only repository-content permission boundary at workflow level.

### B. base ci.yml

`.github/workflows/ci.yml` has no top-level `permissions:` declaration in the audited source.

It also does not directly reference `${{ secrets.GITHUB_TOKEN }}` in the inspected workflow.

Effective default `GITHUB_TOKEN` permissions for a run are partly a repository/organization Actions setting and are not established by this source file alone.

Classification:

- source-level explicit permissions: ABSENT;
- effective account/repository default permission setting at audited time: TOKEN_VAZIO in this grain;
- direct token-driven repository mutation in ci.yml: NOT_OBSERVED.

### C. tag.yml remote mutation path

`tag.yml` triggers on:

`push.tags: "*"`

It does not declare an explicit `permissions:` block in the audited workflow.

The upload step passes:

- `GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}`;
- `GITHUB_TAG: ${{ github.ref }}`.

It then executes:

`.github/workflows/upload_github_release_asset.py`.

That Python helper can:

- list tags and releases;
- create a GitHub release;
- upload an asset;
- delete a matching asset after a partial/failed upload before retrying.

Canonical activation chain:

`tag push -> tag.yml -> GITHUB_TOKEN -> PyGithub -> GitHub release API -> create/upload/delete asset`.

This mutation path is tag-event scoped by the audited YAML. It is not activated by an ordinary branch push through this workflow.

### D. unresolved token permission

Because `tag.yml` does not define `permissions:`, the exact effective permission granted to `secrets.GITHUB_TOKEN` depends on GitHub repository/organization Actions policy outside the commit.

The currently available repository-source interface does not expose that settings endpoint in this audit path.

Therefore:

`TAG_GITHUB_TOKEN_EFFECTIVE_PERMISSION = TOKEN_VAZIO_EXTERNAL_SETTING`

No inference is made from whether a historical upload succeeded or failed unless a specific run is correlated separately.

### E. workflow-to-build environment crossings

Observed source-controlled environment values include:

Base CI:
- `BLAKE3_CI=1`;
- `RUSTFLAGS=-D warnings`;
- `RUST_BACKTRACE=1`;
- `RAYON_NUM_THREADS=1`;
- multiple `CFLAGS` variants;
- `RUSTC_BOOTSTRAP=1` in selected tests;
- matrix-selected `CC`, `CXX`, target, OS, CMake and TBB flags;
- `PKG_CONFIG_PATH` written into `GITHUB_ENV`.

RMR workflows:
- explicit `CC=clang` / `CXX=clang++` in relevant jobs;
- benchmark `ROUNDS`, `TARGET_MIB`, `SIZES`, `RESULT_ROOT`;
- canonical `OFFICIAL_REF=6aab490a26124663329dfd3961b8469f8fdb158b` in comprehensive V3;
- checkout ref `${{ github.event.pull_request.head.sha || github.sha }}`;
- final summary receives `needs.<job>.result` values through environment variables.

No `${{ vars.* }}` use was observed in the audited workflow set.

The only secret reference observed in the workflow set is the tag-release `secrets.GITHUB_TOKEN`.

### F. environment authority chain

Relevant general form:

`GitHub event/context -> workflow env/matrix -> shell/Python -> Cargo/CMake/build.rs -> compiler/linker/runtime`.

Examples:

- workflow `CC=clang`
  -> script inherits `CC`
  -> Cargo `build.rs` / `cc` crate discovers compiler;

- workflow `OFFICIAL_REF=<SHA>`
  -> sourced `common.sh`
  -> `git fetch --depth 1 origin "$OFFICIAL_REF"`;

- workflow `CFLAGS=...`
  -> root `build.rs` observes/mutates CFLAGS
  -> native compiler invocation;

- workflow matrix `BLAKE3_FETCH_TBB=YES`
  -> CMake
  -> dependency child
  -> optional oneTBB FetchContent.

## μ2_EXECUTE_OBSERVE — classification

Observed:
- explicit read-only permissions on RMR workflows: YES;
- explicit permissions on base ci.yml: NO;
- explicit permissions on tag.yml: NO;
- secret use outside tag.yml: NOT_OBSERVED;
- repository-variable `${{ vars.* }}` use: NOT_OBSERVED;
- tag-triggered authenticated remote mutation path: OBSERVED;
- effective default token permission for tag.yml: TOKEN_VAZIO_EXTERNAL_SETTING;
- ordinary branch-push activation of tag release workflow: NOT_SUPPORTED_BY_TRIGGER;
- workflow environment influencing build/source/tool selection: OBSERVED.

No evidence here establishes:
- hidden repository secret names beyond the referenced built-in token;
- unauthorized token use;
- a secret-controlled normal-push backdoor;
- encrypted environment commands.

## μ3_CLOSE — custody result and next route

### Result

The RMR workflow family is substantially stricter than the inherited/base workflows at the token-permission layer because RMR files explicitly reduce `contents` to read.

The release workflow is the primary observed GitHub-API write surface.

Its event is explicit (tag push), but its exact token privilege is external to the commit because no `permissions:` contract is declared there.

### Governance requirement

For hermetic authority receipts, record for every run:

- event name;
- ref and exact SHA;
- workflow file/blob SHA;
- declared permissions block;
- effective GitHub token permission summary where retrievable;
- secret/variable names referenced, never secret values;
- environment keys passed to each step;
- source/tool override variables;
- remote mutations performed and returned object IDs.

### R3

- F_ok: source-level token/permission/env authority map recorded.
- F_gap: effective repository Actions default permissions are TOKEN_VAZIO.
- F_next: correlate high-impact edges with historical workflow runs and distinguish POSSIBLE_EDGE from EXECUTED_EDGE.
