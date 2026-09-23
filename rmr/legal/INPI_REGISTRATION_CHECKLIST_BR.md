<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Operational checklist; verify current INPI requirements before filing.
-->

# INPI software registration checklist — RMR

State: PREPARATION_CHECKLIST

Brazilian software protection does not depend on registration, but registration may strengthen evidence of authorship/titularity in disputes.

## Version package

For every version intended for registration:

- canonical product/module name;
- author and rights-holder identity;
- exact Git commit;
- release timestamp;
- source tree manifest;
- provenance/license matrix;
- cryptographic digest of the source material required by the current INPI e-Software procedure;
- Declaration of Veracity;
- qualified digital signature/certificate required by the current INPI flow;
- filing/payment receipt;
- published certificate/process number when issued.

## RMR separation

The filing package must not represent upstream BLAKE3 as RMR-authored.

Prepare a manifest such as:

~~~text
RMR_AUTHORED = rmr/** plus nominal external RMR files in PROVENIENCE.md
UPSTREAM = src/**, c/**, reference_impl/**, test_vectors/**, ...
THIRD_PARTY = provider/dependency material under its own license
~~~

## Technical receipt

Recommended append-only receipt:

~~~text
registration_candidate_id
git_commit
source_manifest_sha256
source_manifest_blake3
license_matrix_sha256
provenance_sha256
filing_timestamp
INPI_process = TOKEN_VAZIO until filed
certificate = TOKEN_VAZIO until issued
~~~

Before filing, re-check the current INPI e-Software guide because forms, accepted signatures and administrative procedures can change.
