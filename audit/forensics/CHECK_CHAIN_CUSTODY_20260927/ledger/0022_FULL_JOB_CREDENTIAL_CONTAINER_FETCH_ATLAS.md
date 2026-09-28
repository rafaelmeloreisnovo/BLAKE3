# μ0022 — Full job pagination, credential inheritance and container/fetch execution

- date: 2026-09-27
- parent_commit: `31615d08b5ad4ffc17767ccc4d29d052a25f55f8`
- audited_commit: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- base_ci_run: `36287272113`
- kind: EXECUTION_CONTEXT_AND_CREDENTIAL_INHERITANCE
- completion_state: PARTIAL_BYTES
- claim_allowed: false

## μ0_START — close the μ0020 pagination gap

μ0020 recorded that the convenience workflow-jobs wrapper returned only 30 jobs.

This grain queried the paginated run subresource directly with `per_page=100`.

Observed:

- `total_count=74`;
- returned jobs: `74`.

Therefore:

`BASE_CI_JOB_ENUMERATION_36287272113 = COMPLETE_74_OF_74`

This append-only record extends μ0020; it does not rewrite the earlier page-bounded observation.

## μ1_RESOLVE — newly reachable execution edges

### A. addnab/docker-run-action job

Job:
- id: `108530284441`;
- name: `compile and test with GCC 5.4`;
- conclusion: success.

Runtime log resolved:
- `actions/checkout@v4` -> `11d5960a326750d5838078e36cf38b85af677262`;
- `addnab/docker-run-action@v3` -> `4f65fabd2431ebc8d299f8e5a018d79a769ae185`.

The action container itself was built from:

`docker.io/library/docker:20.10@sha256:2967f0819c84dd589ed0a023b9d25dcfe7a3c123d5bf784ffbb77edf55335f0c`.

The workflow action then selected `image: gcc:5.4`.

The runtime pulled it and recorded:

`gcc:5.4 -> sha256:e6ef7f0295b9d915f8521de360e30803bf8561cfb9cea8e320aa66761be8ec42`.

Inside that route the configured command included:

`curl https://sh.rustup.rs -sSf | sh -s -- -y --profile minimal`

and then Cargo tests.

Thus the historical digest of two mutable container tags can be reconstructed from logs even though the workflow source did not pin those image digests.

### B. container environment and mounts

The action's Docker invocation passed a broad GitHub/Actions execution context into the container, including named environment keys for:

- GitHub repository/ref/run/action/workflow identity;
- runner paths and architecture;
- Actions runtime/cache/results endpoints/tokens;
- step state/output/path files.

It mounted:

- Docker socket `/var/run/docker.sock`;
- runner temp/home/workflow/file-command paths;
- repository workspace at `/github/workspace`.

The same job logged:
- broad-write `GITHUB_TOKEN` permissions;
- `actions/checkout@v4` with `persist-credentials: true`.

Therefore external/container-executed code in this job had filesystem visibility of the checked-out workspace while checkout credentials were persisted there.

Classification:

`WORKSPACE_WITH_PERSISTED_GIT_AUTH_MOUNTED_INTO_ACTION_CONTAINER = OBSERVED`

`EXTERNAL_CODE_USED_PERSISTED_GIT_AUTH = NOT_PROVEN`

The Docker command excerpt did not explicitly pass a `GITHUB_TOKEN` variable by that name into the action container. The risk boundary exists through persisted workspace Git authentication and other runtime tokens; actual credential use is not inferred.

### C. Windows oneTBB FetchContent route

Full pagination exposed Windows jobs outside the first 30.

Example:
- job `108530284472`;
- `CMake windows-latest CC=cl CXX=cl TBB=ON`;
- conclusion: success.

Runtime command:

`-DBLAKE3_USE_TBB=ON -DBLAKE3_FETCH_TBB=YES`.

Source `c/dependencies/tbb/CMakeLists.txt` defines:

- repository: `https://github.com/uxlfoundation/oneTBB`;
- Git tag/commit: `0c0ff192a2304e114bc9e6557582dfba101360ff`;
- shallow fetch enabled.

Runtime log then shows compilation paths under:

`build/_deps/tbb-src`

and links `tbb12.dll`, followed by installation of TBB headers/libraries.

Combined source + execution evidence supports:

`WINDOWS_TBB_FETCHCONTENT_ROUTE = EXECUTED`

with source-level upstream commit pinned to `0c0ff192...`.

The log excerpt does not independently print the fetched Git HEAD, so byte-for-byte fetched-tree verification remains a separate custody step.

### D. get-cmake action and mutable tool selector

The Windows TBB job used:

`lukka/get-cmake@5f6e04f5267c8133f1273bf2103583fc72c46b17`

which is source-pinned to an action commit.

However the workflow input says:

`cmakeVersion: latest`
`ninjaVersion: latest`.

In job `108530284472`, the action resolved CMake to `3.31.5`.

Therefore:

`ACTION_CODE_PINNED != TOOL_VERSION_PINNED`.

The action implementation was pinned; the selected tool version was mutable at source but recoverable from this run's log.

## μ2_EXECUTE_OBSERVE — authority composition

For the audited base CI run, one can now reconstruct this high-impact route:

`push`
 -> `ci.yml`
 -> broad-write GitHub token context
 -> checkout with persisted credentials
 -> external action by mutable major tag
 -> action resolved to exact SHA
 -> container action with Docker socket + mounted workspace + Actions runtime context
 -> mutable image tag resolved to exact digest
 -> remote rustup script piped to shell
 -> Cargo build/test and further package downloads.

Separately:

`push`
 -> Windows CMake matrix
 -> pinned get-cmake action
 -> mutable "latest" CMake selector -> observed 3.31.5
 -> `BLAKE3_FETCH_TBB=YES`
 -> pinned oneTBB Git commit from source
 -> `_deps/tbb-src`
 -> compile/link TBB DLL + BLAKE3.

No evidence establishes malicious use, command encryption, or intentional concealment.

The forensic issue is that **authority and executable provenance are distributed across layers**, so reading only the workflow line is insufficient.

## μ3_CLOSE — custody state

### Proven/recoverable for run 36287272113

- complete base-CI job list: 74/74;
- checkout action SHA in sampled jobs;
- rust-toolchain action SHA in sampled job;
- addnab action SHA;
- get-cmake action SHA;
- cargo-xwin image digest;
- gcc:5.4 image digest;
- docker:20.10 action-base digest;
- Wasmtime selected version;
- cross selected version;
- CMake selected version in sampled Windows job;
- oneTBB source commit declared by audited source and fetched/built route evidenced.

### Remaining byte gaps

- exact bytes/hash of rustup remote installer;
- exact bytes/hash of Wasmtime installer and archive;
- MSVC CRT downloaded by cargo-xwin;
- complete crate package-byte set per job;
- independent hash of fetched oneTBB worktree;
- full environment/credential exposure per every one of 74 jobs;
- proof of whether any external child read persisted Git credentials.

### R3

- F_ok: 74/74 job enumeration closed; container/fetch routes promoted with historical evidence.
- F_gap: several externally fetched byte artifacts remain TOKEN_VAZIO.
- F_next: create immutable external-dependency ledger with source ref, run-resolved SHA/digest/version, authority exposure and missing-byte receipt for every external edge.
