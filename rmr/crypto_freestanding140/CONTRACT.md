# Crypto Freestanding 140 — Kernel Contract V1

## Runtime invariants

Production kernels must not depend on:
- libc;
- heap allocators;
- garbage collection;
- filesystem;
- environment variables;
- clocks;
- network;
- dynamic loading;
- provider APIs;
- native framework callbacks.

Inputs/outputs are caller-owned buffers and fixed-width values.

## Source-shape target

Under `kernel/`:

```text
if(statement)      = forbidden in V1 hot kernels
for(statement)     = forbidden in V1 hot kernels
while(statement)   = forbidden in V1 hot kernels
malloc/free        = forbidden
variable-length tail loop = forbidden when a fixed-domain kernel can be specialized
hidden provider call = forbidden
```

Control selection should use:
- masks;
- fixed unrolling;
- table-free Boolean/arithmetic transforms when the standard permits;
- compile-time specialization;
- separate specialized functions.

This policy must never change the specified algorithm.

## Allowed operation classes

Per algorithm/specification:
- XOR/AND/OR/NOT;
- shifts/rotates;
- modular integer addition/subtraction;
- fixed-width multiplication when required;
- finite-field arithmetic when required;
- fixed permutations/substitutions;
- constant-shape memory access where required and feasible.

“Only XOR” is not a universal contract.

## Comments

Comments carry human contracts only.

Compiler/linker effects must be expressed through actual:
- macros/directives;
- attributes;
- section placement;
- visibility;
- optimization flags;
- linker flags/scripts.

A comment is never accepted as proof of an optimization.

## Tails and shadows

- tails are removed only when a fixed-size specialization makes them unnecessary;
- lexical shadowing is rejected with `-Wshadow`;
- parallel lanes/SIMD are called lanes, not “shadow variables”;
- duplicate state may be used only when it increases verified parallelism without changing semantics.

## Q profiles

Q8/Q16/Q32/Q42 belong to non-cryptographic measurement/orchestration unless an RMR-specific algorithm explicitly defines fixed-point semantics.

Standard crypto uses its normative bit widths.

## Failure state

If a primitive cannot satisfy a requested source-shape rule without changing semantics:

```text
state = BLOCKED_STYLE_CONSTRAINT
```

not “implemented”.
