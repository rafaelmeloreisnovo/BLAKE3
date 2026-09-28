# μTRACE 0x00000002 — MIDDLE / negative-search reliability gate

parent_mu=0x00000001
phase=02_MIDDLE
evidence_state=OBSERVED
claim_allowed=false

## Experiment
Repository code search was queried for known execution tokens including:
git clone | git fetch | curl | wget | cargo install | pip install | docker run | source | eval | base64

## Control result
The indexed search returned zero hits for terms including `git clone`, `curl`, and `source`.

Direct blob reads performed immediately before this record prove those strings exist, including:
- rmr/upstream_validation/common.sh: git clone + git fetch
- rmr/tools/orchestrate_blake3_compare_v2.sh: git clone + git fetch
- .github/workflows/ci.yml: curl, cargo install, docker run
- rmr/tools/benchmark_rust_cli_v3.sh: source common.sh

## Forensic consequence
CODE_SEARCH_ZERO != REPOSITORY_ABSENCE
SEARCH_INDEX_NEGATIVE=NON_AUTHORITATIVE
BLOB_READ=AUTHORITATIVE_FOR_FILE_CONTENT
TREE_ENUMERATION=AUTHORITATIVE_FOR_PATH_SURFACE

No negative conclusion about tails/shadows/dependencies may be based solely on indexed code-search zero results.

mu_record=0x00000002|0x00000001|02_MIDDLE|GitHub code-search vs direct blobs|validate negative-search reliability|OBSERVED|index incompleteness|enumerate and read reachable blobs directly
