# RMR FIXED256 contract

## Purpose

A deliberately tiny cross-language low-level kernel for a fixed 256-byte frame.
It is **not BLAKE3**, not a cryptographic hash, and not a replacement for any upstream primitive.

## Runtime contract

- Input is exactly 256 bytes = 16 lanes × 16 bytes.
- Output is exactly 16 bytes.
- Output byte `j` is the XOR of byte `j` from each of the 16 lanes.
- No variable-length input and therefore no remainder/tail path exists in the kernel.
- ASM implementations have no heap, libc, syscall, filesystem, clock, threads, dynamic loader or external symbol dependency.
- ASM hot path is assembler-expanded/unrolled; there is no data-dependent branch in the reducer.
- Java kernel uses only primitive arithmetic and caller-owned `byte[]`; no imports, streams, collections, reflection, native bridge, third-party library or allocation inside `reduce`.
- Java is not bare metal: execution still requires a JVM and inherits JVM bounds checks, GC/runtime and JIT/AOT semantics.

## Shadow/friction boundary

"Tail shadow" is treated here only as an engineering constraint on this kernel: no variable tail block, deferred remainder pass or hidden follow-up work is allowed. This module does not claim to control CPU front-end shadow-branch mechanisms.

## Compatibility boundary

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`.
This module must never label its 16-byte reducer output as BLAKE3, SHA-256, CRC32C or any other cryptographic digest.
