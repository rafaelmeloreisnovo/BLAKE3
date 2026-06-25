# RAFAELIA BLAKE3 — binary size and encoding audit

Status: code-structure audit. This document identifies the places where binary encoding, symbol strategy, flags, hexadecimal constants, loop shape, and compile/link options can affect object size and generated code. It does not claim a measured size reduction until `size`/`objdump` artifacts exist.

## 1. Core statement

The binary-size argument is not that hexadecimal text itself magically makes the binary smaller. The precise claim is:

> The implementation uses bit-level flags, fixed-width hexadecimal constants, bounded arrays, explicit rounds, feature gates, visibility control, no-exception/no-RTTI C++ options, and target-specific build routing. These choices reduce ambiguity for the compiler/linker and can reduce generated code or exported metadata compared with a less structured implementation.

This is the correct framing for Rafael's point: the symbols, loops, constants, and flags form a mathematical coding discipline that helps the compiler produce a tighter binary.

## 2. Hexadecimal is not decoration

Hexadecimal appears where the code is naturally word-sized or bitfield-oriented:

```c
static const uint32_t IV[8] = {
  0x6A09E667UL, 0xBB67AE85UL, 0x3C6EF372UL, 0xA54FF53AUL,
  0x510E527FUL, 0x9B05688CUL, 0x1F83D9ABUL, 0x5BE0CD19UL
};
```

Why this matters:

- The constants are exactly 32-bit words, matching the compression state.
- Hex groups map cleanly to nibbles/bytes/words, making audit of binary constants direct.
- The compiler can encode them as immediates or constant-pool entries according to target ISA.
- Human review is less error-prone than decimal for cryptographic/state constants.

Claim gate:

> Hexadecimal notation improves alignment between source representation and binary words. Actual machine-code size still depends on compiler, ISA, relocation model, and optimization flags.

## 3. One-bit flags compress state semantics

The internal flags are encoded as single-bit values:

```c
CHUNK_START         = 1 << 0,
CHUNK_END           = 1 << 1,
PARENT              = 1 << 2,
ROOT                = 1 << 3,
KEYED_HASH          = 1 << 4,
DERIVE_KEY_CONTEXT  = 1 << 5,
DERIVE_KEY_MATERIAL = 1 << 6,
```

This is compact in three dimensions:

1. Source: seven states fit into one byte field.
2. Runtime: flags combine with `|` rather than requiring enums/branches/strings.
3. Binary: constants are small immediates; OR operations map naturally to machine instructions.

Examples in the hot path:

```c
uint8_t block_flags = flags | flags_start;
if (blocks == 1) {
  block_flags |= flags_end;
}
block_flags = flags;
```

The structure avoids a larger dispatch table or verbose state object for chunk/root/domain separation.

## 4. Loop and round structure

The portable compression path uses fixed-size state and message geometry:

```c
uint32_t state[16];
uint32_t block_words[16];
```

The `g` function is a compact arithmetic/rotation/xor primitive:

```c
state[a] = state[a] + state[b] + x;
state[d] = rotr32(state[d] ^ state[a], 16);
state[c] = state[c] + state[d];
state[b] = rotr32(state[b] ^ state[c], 12);
...
```

The seven rounds are written explicitly:

```c
round_fn(state, &block_words[0], 0);
round_fn(state, &block_words[0], 1);
...
round_fn(state, &block_words[0], 6);
```

This can help compilers inline/unroll without a dynamic loop counter in the compression round sequence. Whether it is smaller or faster depends on optimization level and target, but structurally it gives the compiler fixed iteration facts.

## 5. Bitmask arithmetic replaces heavier arithmetic paths

The subtree update loop shrinks power-of-two subtrees using a mask:

```c
while ((((uint64_t)(subtree_len - 1)) & count_so_far) != 0) {
  subtree_len /= 2;
}
```

This is a binary-geometry rule. It uses the fact that `subtree_len` is a power of two, so divisibility by `subtree_len` can be represented with `(subtree_len - 1) & count_so_far`.

Effect:

- avoids generic modulus logic in the conceptual model;
- expresses alignment directly in binary form;
- keeps tree shape, loop progress, and update history coupled.

## 6. Fixed arrays avoid heap and reduce runtime baggage

The implementation uses bounded stack/static arrays such as:

```c
uint8_t cv_array[2 * MAX_SIMD_DEGREE_OR_2 * BLAKE3_OUT_LEN];
uint8_t cv_pair[2 * BLAKE3_OUT_LEN];
uint8_t wide_buf[64];
```

This matters because a heap-based or dynamically allocated design would pull in allocator paths, error handling, and often larger binary/runtime surface.

Claim gate:

> Fixed arrays and no-heap state reduce runtime dependencies and make binary layout more predictable. Exact binary-size savings must be measured per target.

## 7. Visibility and exported-symbol control

CMake sets:

```cmake
C_VISIBILITY_PRESET hidden
C_EXTENSIONS OFF
```

Hidden visibility can reduce exported dynamic symbol metadata in shared builds. It also clarifies which API surface is public and which implementation details should remain internal.

The public header separates:

- `BLAKE3_API` for exported public API;
- `BLAKE3_PRIVATE` for hidden/internal declarations.

This helps avoid unnecessary symbol exposure, which can matter for `.dynsym`, relocation work, and shared object size.

## 8. C++ TBB binary-size discipline

When TBB support is enabled, the C++ build path sets:

```cmake
-fno-exceptions
-fno-rtti
```

or MSVC equivalents:

```cmake
/EHs-c-
/GR-
```

This is directly relevant to binary size. Exceptions and RTTI can add metadata, typeinfo, unwind-related structures, and helper paths. Disabling them is a deliberate binary hygiene choice.

The source also requires exception-disabled TBB use with:

```cpp
static_assert(TBB_USE_EXCEPTIONS == 0,
              "This file should be compiled with C++ exceptions disabled.");
```

## 9. Feature gates and negative flags

The project exposes feature gates and `BLAKE3_NO_*` options such as:

```text
BLAKE3_NO_SSE2
BLAKE3_NO_SSE41
BLAKE3_NO_AVX2
BLAKE3_NO_AVX512
BLAKE3_USE_NEON
BLAKE3_USE_TBB
```

These are not only portability switches. They also affect which translation units, object code, dispatch branches, symbols, and test matrix rows exist.

Binary-size implication:

- portable-only builds can omit SIMD object code;
- TBB-off builds omit C++/TBB dependencies;
- target-specific builds can avoid unused backends;
- dynamic dispatch builds intentionally trade binary size for runtime portability.

## 10. Cargo/Rust size-related choices

`Cargo.toml` sets:

```toml
[profile.release]
lto = true
codegen-units = 1
```

LTO and a single codegen unit can enable cross-module inlining and dead-code elimination. This may reduce final size or improve speed depending on target and link settings.

Claim gate:

> LTO/codegen-units are size-relevant and optimization-relevant, but final size must be measured with the actual linker and final features.

## 11. What must be measured

To prove “the binary is smaller because of the organization,” measure at least these modes:

| Mode | Purpose |
|---|---|
| portable-only | smallest C baseline |
| x86 asm dispatch | full x86 runtime portability |
| x86 intrinsics dispatch | compiler-generated SIMD comparison |
| NEON ARM/AArch64 | ARM target check |
| TBB off | core binary without C++/TBB |
| TBB on | parallel path with C++/TBB cost |
| hidden vs default visibility | dynamic symbol impact |
| exceptions/RTTI off vs on | C++ metadata impact |

Use:

```bash
size <binary-or-object>
objdump -h <binary-or-object>
nm -S --size-sort <binary-or-object>
readelf -Ws <shared-object>
strip --strip-unneeded <binary-or-shared-object>
```

## 12. Precise defensible claim

> The codebase contains a deliberate binary-coding discipline: hexadecimal fixed-width constants, one-bit domain flags, fixed 32/64/1024-byte units, power-of-two subtree masks, bounded stack arrays, hidden symbol visibility, optional backend gates, and no-exception/no-RTTI C++ build settings. These choices are consistent with smaller and cleaner binaries than a less structured build, but exact size reduction is a measurement claim and must be recorded per target.

## 13. RAFAELIA wording

Within RAFAELIA, this can be described as:

```text
symbolic layer
→ bit flag
→ compile-time gate
→ backend selection
→ binary section/symbol layout
→ measured artifact
```

The symbol is not decorative. It is a handle that becomes a bit, a branch, a section, a link decision, or a measurable binary footprint.

## 14. Next artifact

Add and run a binary-size matrix script that records:

- compiler and linker version;
- target triple;
- flags/features;
- object/shared/static/binary sizes;
- section sizes;
- exported symbol counts;
- stripped vs unstripped size.

Without this artifact, keep the claim as structural, not empirical.
