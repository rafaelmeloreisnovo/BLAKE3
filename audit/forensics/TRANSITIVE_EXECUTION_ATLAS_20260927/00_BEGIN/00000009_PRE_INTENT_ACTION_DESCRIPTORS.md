# μTRACE 0x00000009 — PRE_INTENT / FUNCTION CONTROL_GRAFO action descriptors

parent_mu=0x00000008
phase=PRE_INTENT
function=CONTROL_GRAFO
procedure_id=ACTDESC-0001
actor=ChatGPT_GitHub_connector
responsibility=custody_of_analysis_actions
timestamp_local=2026-09-27T21:31-03:00
claim_allowed=false
source_mutation=false

## PRE_INTENT
Before reading any third-party action descriptor, record the intended operation.

INTENT:
Resolve the exact action descriptors at the acquisition-time SHAs already sealed in μTRACE 0x00000008, then enumerate:
- action runtime kind (node/docker/composite)
- main/pre/post entrypoints
- declared inputs and defaults
- environment propagation
- shell execution
- network/package/download edges
- nested actions
- generated/bundled code boundary
- any encoded/minified/generated payload as SHADOW_CANDIDATE only until explained

TARGETS:
actions/checkout@11d5960a326750d5838078e36cf38b85af677262
dtolnay/rust-toolchain@02cb101ec7c40f2c49e1d9714d64511d8e1b74de
addnab/docker-run-action@4f65fabd2431ebc8d299f8e5a018d79a769ae185
lukka/get-cmake@5f6e04f5267c8133f1273bf2103583fc72c46b17

EXPECTED_OUTPUT:
A separate OBSERVE record after acquisition. Absence/failure remains TOKEN_VAZIO; no inference fills it.

syslog=PRE_INTENT|0x00000009|ACTDESC-0001|targets_sealed|next=read_exact_action_descriptors
