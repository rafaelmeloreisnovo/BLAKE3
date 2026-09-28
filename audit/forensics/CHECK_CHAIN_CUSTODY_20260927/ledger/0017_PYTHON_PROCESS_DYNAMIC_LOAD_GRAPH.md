# μ0017 — Python process, dynamic-load and remote-action graph

- date: 2026-09-27
- parent_commit: `b8e1b2ed00d7b8c97714ae40cd7e8493f33bde3b`
- audited_tree: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- kind: PYTHON_TRANSITIVE_EXECUTION_GRAPH
- python_files_enumerated: 35
- completion_state: PARTIAL
- claim_allowed: false

## μ0_START — input and scope

The audited Git tree contains 35 `.py` files.

The forensic question for this grain is narrower than semantic correctness:

1. can Python launch a child process?
2. can Python load code indirectly/dynamically?
3. can Python perform remote mutations or network operations?
4. can environment-controlled values select executable behavior?
5. is any encoded/encrypted payload decoded and executed?

Search and direct-source inspection covered process-launch, dynamic import, network/client APIs, archive/loaders, shell execution and filesystem mutation surfaces.

`SOURCE != EXECUTION != EVIDENCE != CLAIM`

## μ1_RESOLVE — discovered edges

### A. subprocess launch surfaces

Observed Python process launchers include:

- `.github/workflows/build_b3sum.py`
  - imports `subprocess`;
  - executes `cargo build --target <argv> --release` with an argv array and repository `b3sum` cwd.

- `c/test.py`
  - imports `subprocess`;
  - executes the locally built `c/blake3` test binary repeatedly with argv arrays and test-vector input.

- `rmr/benchmark_framework/simperf/tests/test_simperf.py`
  - executes `sys.executable <simperf_model.py> ...`;
  - child interpreter is the Python interpreter of the current runtime.

- `rmr/validation/tests/test_report_completeness.py`
  - executes `sys.executable <report_generator.py> ...`;
  - captures stdout/stderr and asserts generated report semantics.

- `rmr/tools/rmr_topology_audit.py`
  - defines a common `run_tool(argv)` helper;
  - resolves argv[0] with `shutil.which`;
  - launches compiler/readelf/nm/size-style tools through `subprocess.run`;
  - default compiler comes from `--compiler`, whose default is `os.environ.get("CC", "cc")`.

No inspected subprocess call above uses `shell=True`.

### B. local dynamic module loading

Observed fixed-path dynamic imports through `importlib.util.spec_from_file_location` include:

- `rmr/crypto/tests/test_forensic_time_attest.py`
  -> `rmr/crypto/tools/forensic_time_attest.py`;

- `rmr/crypto/tests/test_registry.py`
  -> `rmr/crypto/tools/validate_registry.py`;

- `rmr/crypto/tests/test_zip_custody_profile.py`
  -> `rmr/crypto/tools/validate_zip_custody_profile.py`;

- `rmr/pai42/tests/test_pai42_bridge.py`
  -> `rmr/pai42/tools/pai42_bridge.py`;

- `rmr/tests/test_ata_omega_format.py`
  -> `rmr/tools/ata_decode.py`.

These calls execute Python module top-level code from repository-relative file paths. The paths observed are constructed from `Path(__file__)` and fixed repository-relative names, not from arbitrary network URLs.

### C. GitHub remote mutation surface

`.github/workflows/upload_github_release_asset.py` imports the external `github` package (PyGithub) and:

- constructs `github.Github(os.environ["GITHUB_TOKEN"])`;
- selects `GITHUB_REPOSITORY` and `GITHUB_TAG` from environment;
- lists tags/releases;
- can create a GitHub release;
- uploads a release asset;
- on a failed/partial upload, can enumerate matching assets and delete the matching asset before retrying.

This is an authenticated remote mutation capability.

It is not itself evidence that a normal branch push invokes the release mutation; activation must be tied to the calling workflow/event.

### D. environment-selected executable

`rmr/tools/rmr_topology_audit.py` permits compiler selection through `CC` or explicit `--compiler`.

That creates the chain:

`workflow/shell environment -> Python argument default -> shutil.which(CC) -> subprocess executable`.

This is a real executable-selection shadow. It is configuration indirection, not evidence of malicious replacement.

## μ2_EXECUTE_OBSERVE — forensic classification

Observed:

- Python child-process creation: YES.
- shell=True execution: NOT_OBSERVED in inspected Python launchers.
- `os.system`: NOT_OBSERVED.
- arbitrary `eval/exec` payload execution: NOT_OBSERVED.
- fixed-path `importlib` code loading: OBSERVED.
- authenticated GitHub API mutation in release helper: OBSERVED.
- environment-selected compiler executable: OBSERVED.
- direct requests/urllib/socket client logic in RMR Python utilities: NOT_OBSERVED in this grain.
- base64/decrypt-to-exec chain: NOT_OBSERVED.
- archive-extract-to-exec chain: NOT_OBSERVED.

Important distinction:

`DYNAMIC_IMPORT != REMOTE_CODE_LOAD`

`SUBPROCESS != SHELL_INJECTION`

`REMOTE_MUTATION_CAPABILITY != OBSERVED_ACTIVATION_ON_EVERY_PUSH`

## μ3_CLOSE — result, gaps and next route

### Result

Python is not a passive reporting layer everywhere. It contains:

1. process launchers;
2. local dynamic module loaders;
3. build orchestration;
4. tool discovery;
5. an authenticated GitHub release mutation client.

The strongest Python-side "shadow" for general CI is executable/tool resolution and subprocess fan-out. The release mutation path is stronger in privilege but event-scoped by its workflow.

No encrypted/encoded command methodology was established.

### Remaining gaps

- correlate the release helper with exact `tag.yml` trigger conditions and permissions;
- capture package provenance for PyGithub and its dependency graph;
- map Rust procedural macros/build dependencies that execute during Cargo builds;
- map Cargo registry resolution and lockfile boundaries;
- correlate per-run environment values with the command paths above.

### R3

- F_ok: Python process/dynamic-load/remote-mutation surfaces classified.
- F_gap: exact package byte provenance and per-run activation remain incomplete.
- F_next: Rust build-script/proc-macro/dependency execution graph.
