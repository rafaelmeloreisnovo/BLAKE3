# Buffer/Q16/Cache/Hardware Crosswalk — Termux × Vectra × PCR × Llama

## What the shared method actually is

The repeated RAFAELIA pattern is not merely “use Q16”.

It is:

```text
fixed representation
-> fixed/caller-owned buffer
-> in-place state transition
-> bounded workspace
-> hardware/cache profile
-> specialized lane/backend
-> integrity witness
-> receipt
```

This document is code-first and records contradictions instead of normalizing them away.

## Termux RAFCODEΦ

Observed authority paths:
- `rafaelia/src/main/cpp/zero/include/rafz_pure_q16.h`
- `rafaelia/src/main/java/com/termux/rafaelia/RafaeliaCore.java`
- `app/src/main/cpp/lowlevel/raf_memory_layers.[ch]`
- `docs/RAFAELIA_MEMORY_MODEL.md`

Observed pattern:
- Q16 pure leaf has no libc/heap/I/O/syscall/JNI and forbids runtime `if/for/while/switch/goto/ternary`;
- Q16 multiply uses 64-bit intermediate then right shift;
- mask-based min/max/saturating subtraction;
- fixed unrolled dot8;
- three DirectByteBuffers allocated once: 64 KiB input, 64 KiB output, 64-byte state;
- JNI API works on direct buffers;
- memory profile records 64-byte cache line and Android 16 KiB page model;
- arenas are explicit and separate from Java heap.

Important boundary:
the Java convenience APIs still copy from `byte[]` into the direct input buffer. Therefore “zero-copy” applies to the native JNI buffer boundary, not universally to every public call.

## Vectras-VM-Android

Observed authority paths:
- `engine/rmr/include/rmr_tcg_cache.h`
- `engine/rmr/src/rmr_tcg_cache.c`
- `engine/rmr/include/rmr_neon_simd.h`
- `docs/RAFAELIA_OPERATIONAL_STATE_BUFFER.md`
- `native/rafaelia_q16/`

Observed pattern:
- fixed cache/index arrays rather than per-event heap in the TCG cache object;
- 4 KiB block model and fixed 8 KiB scratch;
- delta-XOR writes touch divergent bits while preserving equal bits;
- hit/miss/coherence/reuse/collapse are state variables;
- NEON/SIMD layer provides bulk XOR/memory/CRC/popcount candidates;
- operational doctrine is Basal + Buffer + Return, with rollback/checkpoint.

Security boundary:
TCG cache behavior is not automatically a secret-key cache. Secret-dependent lookup, replacement, prefetch or address selection would create side-channel risk.

## PCR Rafaelia Code Seed

Observed authority paths:
- `seeds/rafaelia_q16/contract_seed.json`
- `HARDWARE_OPTIMIZATION_GUIDE.md`
- `native/src/core/include/ecc_buffer.h`

Observed pattern:
- exact Q16 transition is pinned as integer semantics;
- 64-bit intermediate + 16-bit shift;
- fixed acceptance vectors/fixed-band receipt;
- hardware guide explicitly models cache lines, tiling, prefetch and SIMD.

Contradiction:
the current ECC buffer API includes `stdlib.h` and exposes create/destroy allocation semantics. It is **not** the no-heap pattern required for CF140.

Therefore PCR supplies useful contracts/hardware methodology, but its ECC buffer must not be used as the freestanding secret-buffer authority without refactoring.

## LlamaRafaelia

Observed authority paths:
- `native/rafaelia_q16/build_rafaelia_q16.sh`
- `rafaelia-baremetal/hardware/raf_hardware.[ch]`
- `rafaelia-baremetal/rafstore/`

Observed pattern:
- Q16 ARM32 runtime uses 64-bit intermediate and explicit shift;
- static/loaderless artifact gates exist;
- hardware layer models architecture, core count and cache-line/cache-size information.

Contradiction:
`rafaelia-baremetal/rafstore/raf_rafstore.c` claims baremetal/no external dependencies in its header text but actually includes `stdlib.h`, `string.h` and uses `malloc/calloc/free`.

Therefore the current RAFSTORE implementation is hosted/heap-backed and must not be inherited into the CF140 secret path.

## Correct CF140 interpretation

The reusable invariant is:

```text
Q16 policy/telemetry != secret key bytes
```

Q16 may represent:
- pressure;
- coherence;
- scheduler weights;
- thresholds;
- error;
- hardware telemetry.

Cryptographic key/state bytes must preserve the primitive's exact bit semantics.

## Secret data path

Target:

```text
trusted entropy adapter
        |
        v
dedicated aligned secret line
        |
        +--> in-place key schedule/workspace
        |
        +--> primitive state
        |
        v
explicit fixed-shape wipe
```

Forbidden in the secret kernel:
- heap;
- strings/logging;
- filesystem;
- Q16 conversion of key material;
- secret-dependent prefetch;
- secret-dependent cache index;
- secret-dependent branch/address when avoidable;
- receipt containing raw key or reversible key-derived material.

## Cache doctrine for secrets

Cache awareness is useful for locality, but cache is not itself a security boundary.

Do:
- align/pad secret workspaces;
- keep fixed access patterns;
- keep public-data prefetch fixed;
- keep secret and telemetry on separate lines where practical;
- bound lifetime;
- wipe scratch and key workspace;
- measure generated code per architecture.

Do not claim:
- “key is safe because it is in cache”;
- “DirectByteBuffer is secure memory”;
- “64-byte alignment prevents cache attacks”.

## Entropy gate

A cryptographic key must not be generated from Q16 recurrence, CRC, timestamps or deterministic RAFAELIA state.

The freestanding kernel may be provider-free, but secure key creation still requires entropy supplied by an explicitly trusted hardware/OS entropy adapter.

If no trusted entropy source exists:

```text
KEYGEN_STATE = BLOCKED_ENTROPY_SOURCE
```
