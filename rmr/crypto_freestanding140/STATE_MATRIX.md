# Crypto Freestanding 140 — State Matrix

## Counts

- seed addresses: **140**
- top-level seed families: **7 × 20**
- RMR runtime algorithms already represented through upstream/provider surfaces: **28 runtime entries**
- new standard primitive implementations claimed by this directory at bootstrap: **0**
- authorial freestanding substrate kernels at bootstrap: **pending next commit**

## State semantics

| State | Meaning |
|---|---|
| REGISTERED_NOT_IMPLEMENTED | Address exists only as registry target |
| REFERENCE_EXISTING_NOT_REIMPLEMENTED | RMR already exposes an upstream/provider surface elsewhere |
| SPEC_PINNED | Exact normative specification/version fixed |
| AUTHORIAL_SOURCE | RMR source written without importing third-party implementation |
| FREESTANDING_OBJECT | Object compiles without hosted runtime dependency |
| LINK_PROVED | Static/no-runtime link gate passed |
| KAT_PASS | Known-answer vectors passed |
| NEGATIVE_PASS | Invalid/boundary inputs tested |
| CROSS_ARCH_PASS | Equivalent result reproduced on declared architectures |
| REVIEWED | Human review completed |
| BLOCKED_STYLE_CONSTRAINT | Requested source shape would change semantics |
| TOKEN_VAZIO | Missing evidence |

## Important distinction

An existing OpenSSL-backed runtime entry is **not** considered an implementation in this directory.

```text
PROVIDER_BACKED != FREESTANDING140
```
