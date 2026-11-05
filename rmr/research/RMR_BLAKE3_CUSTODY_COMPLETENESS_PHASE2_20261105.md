# RMR/BLAKE3 custody completeness — phase 2

**Record:** `RMR-BLAKE3-CUSTODY-PHASE2-20261105`  
**State:** `IMPLEMENTED_UNTESTED`  
**Promotion:** forbidden  
**claim_allowed:** `false`

This additive gate extends the existing PR #187 documentation. It does not report an RMR run, a physical-device execution, independent reproduction, or scientific validation.

## Preserved separation

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`

A content hash protects retained bytes. It does not establish that every expected record exists, that an effect was durably recorded before acknowledgment, that all observers received the same head, or that a physical execution occurred.

## Primary literature read for this delta

| Paper | Directly observed contribution | Boundary |
|---|---|---|
| [ClaimReceipt — arXiv:2609.01992](https://www.alphaxiv.org/abs/2609.01992) | Separates evidence sufficiency from experiment coverage; a prospective manifest defines the expected universe so omissions become detectable. | Coverage is limited to the committed universe; external truth and pre-witness omissions remain outside the proof. |
| [The Acknowledgment Point Is the System — arXiv:2608.17176](https://www.alphaxiv.org/abs/2608.17176) | Durable receipts are acknowledged only after append and the declared synchronization boundary; recovery rejects gaps. | A local signed epoch does not prove global non-equivocation, model execution, or absence of rollback/forks. |
| [Git Hash Chain Malleability — arXiv:2607.02820](https://www.alphaxiv.org/abs/2607.02820) | One logical signed commit can admit byte-distinct valid serializations with different Git hashes. | This is not a hash collision or content forgery; an existing full-SHA pin still identifies its original object. |

## Gate set

### C1 — Expected universe

Before outcomes exist, commit identifiers, retry rules, terminal-state rules, schema version, and canonical serialization profile.

**PASS:** every expected identifier has exactly one terminal receipt or one typed `TOKEN_VAZIO`; no undeclared identifier exists.  
**Falsifier:** remove one terminal receipt while leaving all other hashes intact. The gate must not pass.

### C2 — Evidence before acknowledgment

A receipt labeled durable may be returned only after append and the declared synchronization boundary succeed.

**PASS:** crash/restart preserves an exact contiguous prefix; append or synchronization failure is fail-stop and emits no success receipt.  
**Falsifier:** acknowledge before synchronization and terminate the process. A missing receipt after recovery must block the gate.

### C3 — Non-equivocation

Independently retain or publish signed/checkpointed heads across at least two trust domains.

**PASS:** observers converge on one successor relation, or a conflicting head is detected and preserved as a fork.  
**Falsifier:** serve two valid successors of one predecessor to isolated observers; exchange of heads must expose the fork.

### C4 — Content binding beyond Git identity

Bind as separate fields:

- Git commit ID;
- Git tree ID;
- canonical manifest digest;
- artifact digest;
- BLAKE3/RMR digest;
- verifier version.

**PASS:** independent readback recomputes every digest and handles a Git alias without treating a forge badge as sufficient evidence.  
**Falsifier:** present an equivalent signed Git serialization under another commit ID; the verifier must preserve the alias and compare canonical content.

### C5 — Independent replay

A verifier outside the producer execution reconstructs the manifest and verifies the receipt chain and selected artifact.

**PASS:** it reproduces the digests and typed verdict from retained inputs.  
**Falsifier:** when only the producer's self-report exists, the gate remains `TOKEN_VAZIO`.

## Typed gaps preserved

- queued workflow terminal state;
- expected-universe execution;
- crash/durability test;
- external non-equivocation witness;
- RMR execution;
- independent reproduction;
- physical ARMv7/ARM64 attestation.

## Rollback / supersession

Do not delete this or earlier records. A correction appends a successor naming this record and the failed gate. Until the falsifiers run, `IMPLEMENTED_UNTESTED != PASS`.
