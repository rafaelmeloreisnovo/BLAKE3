<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under ../LICENSE_RMR.
-->

# RMR Hardware + Build Topology V1

State: SOURCE_IMPLEMENTED_PENDING_CI  
Claim allowed: false

## Purpose

This module makes the software-to-hardware path observable without changing the
BLAKE3 primitive. It treats build/runtime concerns as separate typed surfaces
instead of one undifferentiated "optimization" claim.

```text
preprocessor != compiler != linker != binary
source symbol != linked symbol != runtime pointer
logical I/O operation != physical storage IOPS
IO != IOPS
comment != executable behavior
warning != failure
void boundary != missing semantics
SIMD != multicore != thread count
SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM
```

## 15-bit surface topology

```text
bit  0 PREPROCESSOR  conditional compilation, macros, include graph
bit  1 COMPILER      compiler identity, language level, warning surface
bit  2 LINKER        sections, relocations, undefined/defined symbols
bit  3 BINARY        ELF class, endianness, machine, entry point
bit  4 SYMBOL        source candidates and linked symbols
bit  5 POINTER       pointer width, null/alignment metadata, no dereference
bit  6 LOOP          explicit loop topology/counters
bit  7 IO            logical reads/writes and byte counters
bit  8 COMMENT       comment density/annotation surface
bit  9 WARNING       warning directives/diagnostic markers
bit 10 MODULE        parent/child include and directory relationships
bit 11 OVERLAP       bit-mask overlap and duplicate-symbol candidates
bit 12 CONDITION     if/switch/ternary and conditional-control markers
bit 13 COLOR         presentation markers only; never authoritative state
bit 14 IOPS          physical IOPS evidence surface, distinct from logical I/O
```

The bit mask is a routing/indexing device. A set bit means the audit surface is
represented by the topology framework; it does not mean a defect exists.

## C micro-module

`rmr/topology/` provides:

- compile/preprocessor snapshot;
- pointer metadata without dereferencing;
- bit-mask overlap primitive;
- saturating loop counters;
- saturating logical I/O counters.

The counters intentionally record **logical operations**. Physical IOPS requires
elapsed time, kernel/device context, queue depth, cache policy and workload
definition. No IOPS claim is promoted from source counters alone.

## void discipline

`void` is valid at explicit ABI/generic-memory boundaries. It must not be used
as a substitute for an unknown semantic type. The audit reports both total
`void` tokens and `void *` boundaries so later reviews can distinguish:

```text
VOID_BOUNDARY != TOKEN_VAZIO
VOID_POINTER != UNTYPED_PROGRAM
```

## warnings

Warnings are evidence about a compiler/build surface, not proof of failure.
The topology target uses a stricter warning profile than the legacy RMR targets
without forcing those legacy targets to `-Werror`.

## symbols and overlap

The source audit reports candidate global definitions and duplicate candidates.
The binary audit, when given an ELF executable/object, delegates authoritative
linked-symbol/relocation inspection to `nm` and `readelf` when present.

Therefore:

```text
SOURCE_SYMBOL_CANDIDATE != LINKED_SYMBOL
DUPLICATE_CANDIDATE != LINKER_COLLISION
```

## comments and child modules

Quoted includes form explicit child edges between source files. Top-level
directories form module groups. Comment lines are measured separately from
executable lines so documentation density cannot be confused with execution.

## color

The auditor's terminal colors are presentation-only:

- green: observed audit completed without detected overlap candidates;
- yellow: gap or candidate requiring review;
- red: execution/audit error.

`COLOR != STATE`; JSON fields carry the actual state.

## observable validation

```sh
cmake -S rmr -B build/rmr-topology -DRMR_BUILD_TESTS=ON
cmake --build build/rmr-topology --target rmr-topology-selftest
ctest --test-dir build/rmr-topology -R rmr_topology_selftest --output-on-failure

python3 rmr/tools/rmr_topology_audit.py \
  --root rmr \
  --binary build/rmr-topology/rmr-topology-selftest \
  --json-out build/rmr-topology/rmr_topology_audit.json
```

## Promotion gates

- source present: IMPLEMENTED;
- local compile/selftest: NOT_RUN until observed;
- ELF/linker audit: NOT_RUN until observed;
- GitHub Actions exact-head: NOT_RUN until observed;
- physical ARMv7/AArch64/x86 measurements: TOKEN_VAZIO until receipts exist;
- storage IOPS: TOKEN_VAZIO until a defined timed storage workload exists.
