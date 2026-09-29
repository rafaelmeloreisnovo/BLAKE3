# RMR Portable Enforcement + Audit V1

State: EVIDENCE_PROTOCOL / HUMAN_LEGAL_GATE

## Technical chain

```text
source path
-> license-at-source
-> commit SHA
-> source manifest SHA-256
-> artifact SHA-256
-> binary/source comparison
-> alleged use evidence
-> permission/contract lookup
-> counter-evidence
-> human legal review
```

## No automated accusation

Software may detect similarity or missing authorization records. It must not
automatically declare a person/entity an infringer, impose a fine, debit a
payment, or publish an accusation.

## Audit cost categories

Keep distinct:

```text
routine_audit_cost
technical_verification_cost
confirmed_breach_verification_cost
attorney_fee
litigation_cost
court_awarded_cost
damages
contractual_penalty
```

Only a signed agreement or applicable legal process can determine who owes
which category.

## Evidence minimum

- repository and exact commit;
- path-level license identifier;
- file SHA-256 and, where useful, BLAKE3;
- artifact digest;
- compiler/linker versions and flags;
- reproducible test/KAT receipt;
- date/time source;
- contract/order-form version and signatures if any;
- uncertainty and false-positive analysis.

## Fail-closed states

```text
identity uncertain = TOKEN_VAZIO_IDENTITY
path provenance uncertain = TOKEN_VAZIO_PROVENANCE
prior grant uncertain = TOKEN_VAZIO_LICENSE_HISTORY
institutional/commercial status uncertain = TOKEN_VAZIO_USE_STATUS
no signed audit clause = NO_PRIVATE_AUDIT_RIGHT
no signed penalty clause = TOKEN_VAZIO_CONTRACTUAL_PENALTY
damages not proven = TOKEN_VAZIO_DAMAGES
```
