# μTRACE 0x00000000 — BEGIN / forensic acquisition contract

schema=RMR_TRANSITIVE_EXECUTION_ATLAS_V1
trace_id=0x00000000
phase=00_BEGIN
repository=rafaelmeloreisnovo/BLAKE3
base_ref=master
base_sha=5365b389919881f91be4a407ab5b2d094bc1a65c
acquisition_date=2026-09-27
acquisition_clock=TOKEN_VAZIO
claim_allowed=false
source_mutation=false

## Purpose
Reconstruct, bottom-up and grain-by-grain, the transitive execution graph reachable from commit/push/build/test activity: libraries, scripts, sourced files, build scripts, runners, actions, containers, package managers, network fetches, compiler/linker edges, environment-controlled edges, symlinks, hooks, submodules and other executable dependencies.

## Four-phase unit contract
Every forensic unit is represented as:
1. 00_BEGIN — source identity, preconditions, hypothesis, authority.
2. 01_ENTRY — direct entry points/calls.
3. 02_MIDDLE — transitive calls/dependencies/tails/shadows.
4. 03_END — observed result, gaps, classification, next edge.

No terminal aggregate may erase intermediate records.

## Vocabulary
OBSERVED = directly present in source/API/runtime evidence.
DERIVED = follows mechanically from observed edges.
EXTERNAL = crosses repository boundary.
MUTABLE = target is not content-addressed/pinned by immutable identity.
SHADOW_CANDIDATE = behavior not visible at the immediate parent callsite; not a claim of concealment or intent.
TAIL = downstream execution edge after the apparent parent command.
TOKEN_VAZIO = evidence/authority not presently established.
NOT_PROVEN = hypothesis examined without sufficient evidence.

## Invariants
SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM
TOKEN_VAZIO != 0
SHADOW_CANDIDATE != MALICIOUS
INDIRECTION != OBFUSCATION
NETWORK_EDGE != COMPROMISE
IMPLEMENTED_UNTESTED != PASS

## μ record
Each subsequent record carries:
mu_id | parent_mu | phase | source/ref | operation | evidence_state | gap | next

R3=<F_ok: acquisition boundary sealed at exact master SHA; F_gap: transitive graph not yet exhausted; F_next: inventory executable surface>
