# Release Notes — Crypto Freestanding 140 bootstrap

## Added

- complete 140-address source registry copied from the Papers research branch;
- normalized RMR implementation-state registry;
- authorship/authority contract;
- freestanding kernel contract;
- seven family indexes;
- state matrix;
- roadmap.

## Corrected

- XOR is not treated as a universal substitute for required arithmetic.
- Q8/Q16/Q32/Q42 are not injected into standard cryptographic semantics.
- provider-backed implementations are not called freestanding implementations.
- “authorial implementation” is separated from “invention of primitive”.

## Security

No new cryptographic primitive is promoted in this bootstrap commit.

## F_next

Implement and prove the branchless fixed-shape substrate, then port one low-risk exact primitive core at a time behind KAT gates.
