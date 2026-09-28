# μ0015 — Python execution, dynamic import, and API side-effect map

- date: 2026-09-27
- parent_commit: b8e1b2ed00d7b8c97714ae40cd7e8493f33bde3b
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: PYTHON_EXECUTION_GRAPH
- python_files_enumerated: 35
- claim_allowed: false

## Coverage

All 35 `.py` files in the audited tree were enumerated for subprocess spawning, shell execution, dynamic imports, dynamic evaluation, network clients, native-library loading, archive extraction, environment-controlled executable selection, and filesystem-controlled module loading.

## Process-spawning Python

Observed `subprocess.run` users include:
- `.github/workflows/build_b3sum.py` -> invokes `cargo build --target <argv> --release` in `b3sum/`;
- `c/test.py` -> executes the locally built `c/blake3` binary with argument arrays and `check=True`;
- `rmr/benchmark_framework/simperf/tests/test_simperf.py` -> executes the repository simperf model using fixed repository paths;
- `rmr/tools/rmr_topology_audit.py` -> generic `run_tool(argv)` wrapper using `shutil.which(argv[0])`, argument-vector execution, `check=False`;
- `rmr/validation/tests/test_report_completeness.py` -> executes fixed repository report generators in temporary test fixtures.

No `shell=True` was observed in these subprocess paths.

### build_b3sum propagation gap

`build_b3sum.py` invokes `subprocess.run([...])` without `check=True`.

Therefore immediate child nonzero status is not automatically raised at that line. Subsequent file operations may still fail, so this is not proof that a bad build becomes a successful release; it is a child-status propagation weakness.

Classification: `CHILD_FAILURE_PROPAGATION = WEAK_AT_BUILD_CALL`.

## Dynamic module loading

Observed tests use `importlib.util.spec_from_file_location(...); exec_module(...)`.

Inspected paths are derived from `Path(__file__)` and point to fixed repository modules, including:
- `rmr/crypto/tools/forensic_time_attest.py`;
- `rmr/crypto/tools/validate_registry.py`;
- `rmr/crypto/tools/validate_zip_custody_profile.py`;
- `rmr/pai42/tools/pai42_bridge.py`;
- `rmr/tools/ata_decode.py`.

Classification:
- dynamic Python execution mechanism: OBSERVED;
- caller-controlled arbitrary external module path in inspected tests: NOT_OBSERVED;
- repository-local test import indirection: OBSERVED.

## Topology-audit executable selector

`rmr_topology_audit.py` accepts `--compiler`, defaulting to environment `CC` or `cc`.

Its `run_tool(argv)` resolves `argv[0]` through `shutil.which` and executes an argument vector with `shell=False` semantics.

Thus PATH/CC can select the executable implementation. This is an environment/toolchain shadow, not shell-string injection.

## GitHub release side effect

`.github/workflows/upload_github_release_asset.py` uses PyGithub and environment values:
- `GITHUB_TOKEN`;
- `GITHUB_TAG`;
- `GITHUB_REPOSITORY`.

It performs network/API side effects:
- enumerate tags;
- create a GitHub Release when absent;
- enumerate releases;
- upload an asset;
- on a particular failed-upload state, enumerate release assets and delete the same-named partial asset before retrying.

Therefore the tag workflow contains a real repository-side mutation tail below the Python call.

`pip install PyGithub` in `tag.yml` is unversioned, so the API client implementation is externally mutable at runtime.

## Negative Python findings

Across the 35 Python files, this scan did not identify:
- `os.system`;
- `os.popen`;
- `subprocess.Popen`;
- `shell=True`;
- arbitrary `eval` / built-in `exec` of text;
- `ctypes`/dlopen-style native loading;
- pickle/marshal deserialization;
- HTTP downloader code in RMR Python;
- archive `extractall` / unpack execution chain;
- encoded/base64 command decoder.

`exec_module` exists only in the fixed repository-test module loading paths described above.

## Forensic classification

- Python child processes: OBSERVED
- shell-string execution: NOT_OBSERVED
- repository-local dynamic imports: OBSERVED
- external dynamic Python module path: NOT_OBSERVED
- release API mutation: OBSERVED
- environment-selected compiler executable: OBSERVED
- encrypted/encoded Python payload: NOT_OBSERVED
- deliberate concealment: NOT_ESTABLISHED

## R3

- F_ok: all Python files enumerated; process/import/API effects mapped.
- F_gap: Rust/C native dynamic-process/network boundaries and generated Make/CMake children still need static closure.
- F_next: scan Rust for process spawning/build-time effects, then C/C++ for system/exec/dlopen/socket/network APIs.