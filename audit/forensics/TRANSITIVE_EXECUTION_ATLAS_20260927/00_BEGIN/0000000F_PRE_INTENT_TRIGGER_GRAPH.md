# μTRACE 0x0000000F — PRE_INTENT / transverse-longitudinal trigger graph

parent_mu=0x0000000E
phase=PRE_INTENT
function=CONTROL_GRAFO
procedure_id=TRIGRAPH-0004
timestamp_local=2026-09-28T01:40-03:00
claim_allowed=false
source_mutation=false

## INTENT
Build a multidimensional influence graph rather than a single call chain.

LONGITUDINAL AXES:
commit/ref -> workflow event -> job -> step -> action/script -> build system -> compiler/linker -> artifact/runtime

TRANSVERSE AXES:
- workflow-to-workflow coupling
- branch/tag/path filters
- concurrency/cancellation
- matrix expansion and conditional expressions
- permissions/GITHUB_TOKEN/secrets/vars/environments
- artifacts/cache/state transfer
- container/service/image layers
- package managers/registries/downloads
- Cargo features/build.rs/proc-macros/config/runners
- CMake FetchContent/find_package/toolchain selection
- Git config/submodules/LFS/credentials
- release/tag/manual/schedule/API dispatch events
- repository rules/protection/settings where observable

HYPOTHESIS CLASSES:
SHADOW_CANDIDATE = influence not visible at immediate parent callsite
TAIL = downstream causal edge
CROSS_EDGE = transverse influence between otherwise separate chains
ORTHOGONAL_EDGE = shared external state influencing parallel chains
ANOMALY = observed inconsistency requiring explanation
NOT_PROVEN = no evidence yet

No "firewall bypass", covert control, or intentional obfuscation will be asserted without direct evidence.

syslog=PRE_INTENT|0x0000000F|TRIGRAPH-0004|multiaxis_trigger_inventory|next=read_all_workflow_triggers_and_state_transfer
