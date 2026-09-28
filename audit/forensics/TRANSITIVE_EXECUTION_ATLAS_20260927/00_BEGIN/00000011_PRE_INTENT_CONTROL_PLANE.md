# μTRACE 0x00000011 — PRE_INTENT / repository external control plane

parent_mu=0x00000010
phase=PRE_INTENT
function=CONTROL_GRAFO
procedure_id=CTRLPLANE-0005
timestamp_local=2026-09-28T01:40-03:00
claim_allowed=false

INTENT:
Inspect repository-level controls that can influence workflow execution without being encoded inside an individual source file:
- branch protection / rulesets
- Actions repository permissions/default workflow permissions
- environments and deployment gates
- webhooks, if readable
- Actions variables/secrets metadata only if exposed by authorized API; never read or record secret values
- default branch state relevant to event routing

FORENSIC RULE:
REPOSITORY_SETTING != SOURCE_BLOB
CURRENT_SETTING != HISTORICAL_SETTING
Any current-state observation is timestamp-bounded and cannot be projected backwards without history evidence.

syslog=PRE_INTENT|0x00000011|CTRLPLANE-0005|external_control_plane|next=query_authorized_repo_control_endpoints
