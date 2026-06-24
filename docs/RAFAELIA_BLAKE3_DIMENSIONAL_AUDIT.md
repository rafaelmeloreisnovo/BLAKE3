# RAFAELIA BLAKE3 — dimensional audit

Status: code-structure audit. This document analyzes sizes, powers of two, memory layout, ABI-sensitive padding, SIMD degree, subtree geometry, and execution-cycle affinity. It does not claim benchmark results.

## 1. Core statement

The important point is not only that this repository uses BLAKE3. The deeper engineering point is that the implementation is dimensioned around binary-friendly sizes and power-of-two execution geometry:

```text
32-byte CV/hash/key
64-byte compression block / XOF block
1024-byte chunk = 16 blocks
2^54 chunk depth => 2^64 byte addressable input horizon
SIMD degree = 1 / 4 / 8 / 16 depending on backend
lazy merge stack = (MAX_DEPTH + 1) * OUT_LEN
```

These constants are not arbitrary. They define how the code fits the binary execution model: cache lines, vector lanes, stack arrays, subtree splitting, and root-finalization delay.

## 2. Size lattice

| Element | Size | Code meaning | Execution meaning |
|---|---:|---|---|
| `BLAKE3_OUT_LEN` | 32 bytes | Chaining value, default digest, key length | 256-bit unit; 8 x `u32`; compact CV/hash atom |
| `BLAKE3_BLOCK_LEN` | 64 bytes | Compression block and XOF output block | Common cache-line-sized block; 16 x `u32` message words |
| `BLAKE3_CHUNK_LEN` | 1024 bytes | Chunk processed as 16 blocks | Binary pagelet for tree hashing; 16 * 64 |
| `BLAKE3_MAX_DEPTH` | 54 | Tree depth over chunks | `2^54 * 1024 = 2^64` byte horizon |
| `cv_stack` | 1760 bytes | `(54 + 1) * 32` | Lazy merge memory for full 64-bit input space |
| `MSG_SCHEDULE` | `7 * 16` entries | Permutation schedule | 7-round structural rhythm over 16 message words |
| internal flags | 7 one-bit flags | domain separation | chunk/root/parent/key/derive phase encoding |

## 3. `blake3_hasher` memory layout

The C public struct is dimensioned from:

```c
uint32_t key[8];                         // 32 bytes
blake3_chunk_state chunk;                // ABI-dependent; usually 112 bytes on 64-bit
uint8_t cv_stack_len;                    // 1 byte
uint8_t cv_stack[(54 + 1) * 32];          // 1760 bytes
```

Nominal payload before ABI padding on a common 64-bit ABI:

```text
key                 =   32
chunk_state         =  112
cv_stack_len        =    1
cv_stack            = 1760
raw subtotal        = 1905
rounded alignment   = 1912 bytes on common 64-bit ABI
```

This matches the known large-but-fixed hasher profile. The design avoids heap allocation and keeps the merge stack local to the hasher object.

### ABI caveat

Exact `sizeof(blake3_hasher)` is ABI-dependent because `uint64_t` alignment can differ across targets. For documentation, use:

> The hasher has a fixed-size, allocation-free state; on common x86-64 ABIs it is 1912 bytes, dominated by the 1760-byte CV stack.

For Termux/Android/ARMv7, measure with a small `sizeof` probe before publishing exact numbers.

## 4. `blake3_chunk_state` layout

`blake3_chunk_state` contains:

```text
cv[8]              = 32 bytes
chunk_counter      =  8 bytes
buf[64]            = 64 bytes
buf_len            =  1 byte
blocks_compressed  =  1 byte
flags              =  1 byte
payload subtotal   =107 bytes + ABI padding
```

The 64-byte buffer is the direct bridge between partial input and the 64-byte compression block. The `blocks_compressed` counter is a byte because a chunk is 1024 bytes and a block is 64 bytes, so a chunk has only 16 blocks.

## 5. `output_t` layout

The temporary output object stores:

```text
input_cv[8]        = 32 bytes
counter            =  8 bytes
block[64]          = 64 bytes
block_len          =  1 byte
flags              =  1 byte
payload subtotal   =106 bytes + ABI padding
```

On common 64-bit ABIs this also rounds to 112 bytes. This is a compact stack object used to defer root/non-root decisions and produce chaining values or XOF bytes.

## 6. 64-byte block and XOF alignment

`output_root_bytes` works in 64-byte XOF blocks:

```text
output_block_counter = seek / 64
offset_within_block  = seek % 64
wide_buf[64]
```

This means seek/fill behavior is aligned to the same 64-byte unit as compression. When output buffers are multiples of 64 bytes, the implementation avoids repeated partial-block work.

## 7. Chunk geometry: 1024 = 16 * 64

A chunk is exactly 1024 bytes, which is 16 compression blocks. In the hot path, `compress_chunks_parallel` calls:

```c
BLAKE3_CHUNK_LEN / BLAKE3_BLOCK_LEN
```

That is exactly 16. This produces a clean block count for `hash_many`, avoiding variable block geometry in the chunk loop.

## 8. SIMD degree geometry

The implementation has these effective degrees:

| Backend | Degree |
|---|---:|
| Portable | 1 |
| SSE2/SSE4.1 | 4 |
| AVX2 | 8 |
| AVX-512 | 16 |
| NEON | 4 |
| Wasm SIMD | 4 |

The C side also has `MAX_SIMD_DEGREE_OR_2`, because some paths require at least two outputs to avoid accidentally compressing a root too early.

This is a subtle dimensional decision: even when SIMD degree is 1, the tree layer sometimes forces degree 2 as a structural minimum.

## 9. Wide subtree stack arrays

`blake3_compress_subtree_wide` uses:

```c
uint8_t cv_array[2 * MAX_SIMD_DEGREE_OR_2 * BLAKE3_OUT_LEN];
```

Maximum stack buffer by backend:

| Backend class | `MAX_SIMD_DEGREE_OR_2` | `cv_array` size |
|---|---:|---:|
| Portable | 2 | 128 bytes |
| NEON/Wasm/SSE2/SSE4.1 | 4 | 256 bytes |
| AVX2 Rust platform max | 8 | 512 bytes |
| AVX-512 / C x86 max | 16 | 1024 bytes |

`compress_subtree_to_parent_node` uses:

```c
uint8_t cv_array[MAX_SIMD_DEGREE_OR_2 * BLAKE3_OUT_LEN];
uint8_t out_array[MAX_SIMD_DEGREE_OR_2 * BLAKE3_OUT_LEN / 2];
```

So the temporary stack pressure scales with SIMD width but remains bounded and power-of-two shaped.

## 10. Power-of-two subtree splitting

The tree path chooses left subtree size with:

```c
full_chunks = (input_len - 1) / BLAKE3_CHUNK_LEN;
left = round_down_to_power_of_2(full_chunks) * BLAKE3_CHUNK_LEN;
```

The update loop also shrinks `subtree_len` until it evenly divides the count so far, using bitmask logic:

```c
while (((subtree_len - 1) & count_so_far) != 0) {
  subtree_len /= 2;
}
```

This is not just algorithmic elegance. It is a binary-affinity rule: subtree sizes stay power-of-two aligned relative to the input history, preserving correct tree shape while keeping SIMD/multithreading opportunities.

## 11. Lazy merge as execution-cycle control

The implementation intentionally delays merging. It wants to feed as much input as possible into `compress_subtree_wide` without prematurely deciding that a node is root.

Key implication:

```text
larger input window
→ wider subtree compression
→ delayed root decision
→ correct XOF/root flags
→ better SIMD/TBB opportunity
```

The code comments explicitly note the desire to parallelize a 64 KiB input as one subtree when possible, while avoiding accidental root compression.

## 12. Warning gate hidden in dimensions

There is a dimension-specific warning workaround:

```c
#if MAX_SIMD_DEGREE_OR_2 > 2
  ... condense CVs until 2 remain ...
#endif
```

This avoids a GCC warning path when the compile-time dimension says the loop cannot execute. That means warning hygiene is tied to static dimensional facts, not only style.

## 13. Rust/C dimensional consistency

The Rust side mirrors the same lattice:

```rust
OUT_LEN = 32
KEY_LEN = 32
BLOCK_LEN = 64
CHUNK_LEN = 1024
MAX_DEPTH = 54
CVWords = [u32; 8]
CVBytes = [u8; 32]
```

It also documents that `2^54 * CHUNK_LEN = 2^64`, confirming that the tree depth is chosen for a 64-bit input horizon.

## 14. Android/Termux/ARM32 note

For Rafael's ARM32/Termux target, do not copy desktop conclusions blindly.

What matters there:

- NEON availability and ABI flags;
- whether `uint64_t` alignment rounds `blake3_chunk_state` to 108 or 112 bytes;
- stack pressure from `cv_array` on recursive paths;
- compiler choice: Clang vs GCC;
- `-mfpu=neon-vfpv4` and float ABI only when valid for the target;
- thermal behavior on long runs;
- whether TBB is viable or too heavy for the device profile.

## 15. Defensible claim

Precise claim:

> The implementation is dimensioned around a binary execution lattice: 32-byte CVs, 64-byte blocks, 1024-byte chunks, power-of-two subtree splitting, fixed lazy-merge stack, and SIMD-degree-bounded temporary arrays. The fork's documentation should treat these dimensions as operational design constraints, not as incidental constants.

## 16. Next measurement artifact

Add a small C probe for real target sizes:

```c
#include "blake3.h"
#include <stdio.h>
#include <stddef.h>

int main(void) {
  printf("sizeof(blake3_chunk_state)=%zu\n", sizeof(blake3_chunk_state));
  printf("sizeof(blake3_hasher)=%zu\n", sizeof(blake3_hasher));
  printf("offsetof(cv_stack_len)=%zu\n", offsetof(blake3_hasher, cv_stack_len));
  printf("offsetof(cv_stack)=%zu\n", offsetof(blake3_hasher, cv_stack));
  return 0;
}
```

Run this on x86-64, AArch64, and ARMv7/Termux before publishing exact ABI claims.

## 17. Retrofeedback

`F_ok`: size geometry is real and central.  
`F_gap`: exact ABI size still requires target probe on Android/Termux.  
`F_next`: add `tools/sizeof_blake3_state.c` and a benchmark/ABI table.
