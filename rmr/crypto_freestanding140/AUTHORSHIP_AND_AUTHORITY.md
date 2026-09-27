# Authorship and Authority — Crypto Freestanding 140

## Rule

Named cryptographic primitives are treated as external mathematical/specification authorities unless an exact RMR-specific primitive is separately defined and supported.

The new directory's authorial scope is the **implementation expression and methodology**, not a blanket ownership claim over equations or standards.

## Legal/technical caution

This repository does not rely on the blanket statement “formulas cannot be registered” as a complete legal rule. Copyright, patent, trademark, standards licensing and jurisdictional questions are different subjects.

Operational rule:

```text
math/spec authority -> identify
third-party source code -> do not copy without provenance/license review
RMR code written here -> RMR authorial implementation
ambiguous origin -> TOKEN_VAZIO_ORIGIN
patent/licensing question -> HUMAN_LEGAL_REVIEW
```

## No silent derivation

A standard algorithm may be independently implemented from a public normative specification, but:
- the specification must be pinned;
- test vectors must be pinned;
- no external implementation may be copied silently;
- compatibility does not imply authorship of the primitive;
- identical output is required where the standard defines it.

## RMR-specific methodology

The following can be RMR-authored:
- fixed-memory ABI;
- compile/link contracts;
- specialized module partitioning;
- symbol/section policy;
- cross-architecture receipts;
- evidence envelopes;
- branchless fixed-shape kernels where correct;
- negative-control gates;
- rollback/supersession rules.
