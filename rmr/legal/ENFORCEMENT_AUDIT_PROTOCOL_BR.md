<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Documentation/audit protocol; not legal advice.
-->

# RMR enforcement + audit protocol — Brazil

State: OPERATIONAL_EVIDENCE_PROTOCOL / LEGAL_ACTION_HUMAN_ONLY

## Evidence chain

~~~text
SOURCE
 -> provenance
 -> license-at-commit
 -> artifact hash
 -> distribution evidence
 -> alleged use
 -> permission/contract check
 -> technical comparison
 -> human legal review
 -> notice / negotiation / judicial action
~~~

No automated system may declare a person an infringer or impose a fine.

## Audit receipt

Each audit should preserve repository + commit, path-level license/provenance, SHA-256/BLAKE3 artifact digests, source/binary comparison method, timestamps and clock source, compiler/build receipts where relevant, lawfully obtained logs, contract/order-form version and signatures, uncertainties, counter-evidence and false-positive analysis.

## Commercial license audit

If a signed commercial contract grants audit rights, the audit must follow its scope, notice, confidentiality and frequency provisions.

Proposed cost allocation should distinguish:

~~~text
routine_audit_cost
confirmed_material_breach_verification_cost
litigation_cost
attorney_fee
court_awarded_cost
~~~

They are not automatically equivalent.

A future contract may allocate reasonable audit costs after a confirmed material breach, subject to counsel review and applicable law. Litigation and attorney-fee recovery depend on the governing contract, procedure and judicial orders.

## Brazil remedies boundary

Lei 9.609/1998 provides criminal provisions for software copyright violations and separately allows civil action seeking cessation of infringement, a pecuniary measure for violation of the order, and damages.

Those statutory/judicial remedies are distinct from a negotiated contractual penalty.

## Fail-closed states

- identity uncertain -> TOKEN_VAZIO_IDENTITY;
- authorship/path provenance uncertain -> TOKEN_VAZIO_PROVENANCE;
- prior license grant uncertain -> TOKEN_VAZIO_LICENSE_HISTORY;
- commercial status uncertain -> TOKEN_VAZIO_COMMERCIAL_STATUS;
- no signed penalty clause -> TOKEN_VAZIO_CONTRACTUAL_PENALTY;
- damages unproven -> TOKEN_VAZIO_DAMAGES;
- automated accusation -> PROHIBITED_PROCESS.

## Human gates

Legal notices, settlement demands, contract termination, registration filings and litigation require human authorization and, where appropriate, licensed counsel.
