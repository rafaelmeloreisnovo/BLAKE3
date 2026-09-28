# μTRACE 0x00000008 — BEGIN / forensic unit 1 third-party action identity

parent_mu=0x00000007
phase=00_BEGIN
unit=1
evidence_state=OBSERVED_AT_ACQUISITION_TIME
acquisition_date=2026-09-27
acquisition_clock=TOKEN_VAZIO
claim_allowed=false

## Current ref resolution snapshot
At acquisition:
- actions/checkout tag v4 -> 11d5960a326750d5838078e36cf38b85af677262
- dtolnay/rust-toolchain branch master -> 02cb101ec7c40f2c49e1d9714d64511d8e1b74de
- addnab/docker-run-action tag v3 -> 4f65fabd2431ebc8d299f8e5a018d79a769ae185
- lukka/get-cmake workflow already pins 5f6e04f5267c8133f1273bf2103583fc72c46b17; commit exists and GitHub reports valid verification.

## Critical temporal limitation
CURRENT_REF_RESOLUTION != HISTORICAL_RUN_RESOLUTION.
A moving tag/branch may have pointed to a different commit when an older workflow ran.
Therefore these SHAs are a 2026-09-27 acquisition snapshot, not retroactive proof.

## Next
Fetch action descriptors at the resolved commits and expand their own execution entrypoints/dependencies.

mu_record=0x00000008|0x00000007|00_BEGIN|third-party refs|snapshot current ref identities|OBSERVED_AT_ACQUISITION_TIME|historical identities TOKEN_VAZIO|read action descriptors at exact SHAs
