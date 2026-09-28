# μTRACE 0x0000000B — PRE_INTENT / action implementation tails

parent_mu=0x0000000A
phase=PRE_INTENT
function=CONTROL_GRAFO
procedure_id=ACTIMPL-0002
timestamp_local=2026-09-27T21:31-03:00
claim_allowed=false

INTENT:
Before fetching implementation files, seal the next operation:
1. inspect addnab/docker-run-action Dockerfile and entrypoint;
2. inspect package manifests/lock boundaries for node20 actions;
3. inspect human source maps/source directories where available;
4. treat bundled/minified dist files as generated artifacts, not automatically as obfuscation;
5. identify subprocess/network/credential/environment operations;
6. preserve TOKEN_VAZIO where source-to-dist reproducibility is not established.

syslog=PRE_INTENT|0x0000000B|ACTIMPL-0002|implementation_tail_acquisition|next=fetch_exact_implementation_files
