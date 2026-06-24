# BLAKE3 C implementation — fork notes

This directory contains the C implementation of BLAKE3 plus this fork's documented build-orchestration extensions.

The cryptographic core is treated as compatibility-sensitive. Claims about performance, portability, or superiority must be backed by build logs, test vectors, and benchmarks for each target.

## Example

An example program that hashes bytes from standard input and prints the result:

```c
#include "blake3.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);

  unsigned char buf[65536];
  while (1) {
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n > 0) {
      blake3_hasher_update(&hasher, buf, n);
    } else if (n == 0) {
      break;
    } else {
      fprintf(stderr, "read failed: %s\n", strerror(errno));
      return 1;
    }
  }

  uint8_t output[BLAKE3_OUT_LEN];
  blake3_hasher_finalize(&hasher, output, BLAKE3_OUT_LEN);

  for (size_t i = 0; i < BLAKE3_OUT_LEN; i++) {
    printf("%02x", output[i]);
  }
  printf("\n");
  return 0;
}
```

On x86_64 Linux with the assembly implementations:

```bash
gcc -O3 -o example example.c blake3.c blake3_dispatch.c blake3_portable.c \
    blake3_sse2_x86-64_unix.S blake3_sse41_x86-64_unix.S \
    blake3_avx2_x86-64_unix.S blake3_avx512_x86-64_unix.S
```

## Public API summary

### Common API

```c
void blake3_hasher_init(blake3_hasher *self);
void blake3_hasher_update(blake3_hasher *self, const void *input, size_t input_len);
void blake3_hasher_finalize(const blake3_hasher *self, uint8_t *out, size_t out_len);
void blake3_hasher_finalize_seek(const blake3_hasher *self, uint64_t seek, uint8_t *out, size_t out_len);
void blake3_hasher_reset(blake3_hasher *self);
```

### Keyed and derive-key API

```c
void blake3_hasher_init_keyed(blake3_hasher *self, const uint8_t key[BLAKE3_KEY_LEN]);
void blake3_hasher_init_derive_key(blake3_hasher *self, const char *context);
void blake3_hasher_init_derive_key_raw(blake3_hasher *self, const void *context, size_t context_len);
```

### Optional TBB API

When compiled with `BLAKE3_USE_TBB`, the public header exposes:

```c
void blake3_hasher_update_tbb(blake3_hasher *self, const void *input, size_t input_len);
```

This function gives the same hash result as `blake3_hasher_update`, but routes large subtree work through the TBB-enabled internal path.

## Build with CMake

```bash
cmake -S c -B c/build -DCMAKE_BUILD_TYPE=Release
cmake --build c/build
```

With examples:

```bash
cmake -S c -B c/build -DCMAKE_BUILD_TYPE=Release -DBLAKE3_EXAMPLES=ON
cmake --build c/build --target blake3-example
```

With optional oneTBB:

```bash
cmake -S c -B c/build -DCMAKE_BUILD_TYPE=Release -DBLAKE3_USE_TBB=ON
cmake --build c/build
```

If oneTBB is not installed, the CMake logic warns and disables the TBB path unless `BLAKE3_FETCH_TBB=ON` is also enabled.

```bash
cmake -S c -B c/build -DCMAKE_BUILD_TYPE=Release \
  -DBLAKE3_USE_TBB=ON \
  -DBLAKE3_FETCH_TBB=ON
cmake --build c/build
```

## Manual build notes

### x86 dispatch

Dynamic dispatch is enabled by default on x86. `blake3_dispatch.c` selects the best available implementation at runtime among portable C, SSE2, SSE4.1, AVX2, and AVX-512 when those implementations are compiled in.

Intrinsics builds require per-file instruction flags:

```bash
gcc -c -fPIC -O3 -msse2 blake3_sse2.c -o blake3_sse2.o
gcc -c -fPIC -O3 -msse4.1 blake3_sse41.c -o blake3_sse41.o
gcc -c -fPIC -O3 -mavx2 blake3_avx2.c -o blake3_avx2.o
gcc -c -fPIC -O3 -mavx512f -mavx512vl blake3_avx512.c -o blake3_avx512.o
gcc -shared -O3 -o libblake3.so blake3.c blake3_dispatch.c blake3_portable.c \
    blake3_avx2.o blake3_avx512.o blake3_sse41.o blake3_sse2.o
```

Portable-only x86 build:

```bash
gcc -shared -O3 -o libblake3.so \
  -DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512 \
  blake3.c blake3_dispatch.c blake3_portable.c
```

### ARM/NEON

AArch64 enables NEON by default in the implementation. For other ARM targets, enable it explicitly only when the target supports NEON:

```bash
gcc -shared -O3 -o libblake3.so -DBLAKE3_USE_NEON=1 \
  blake3.c blake3_dispatch.c blake3_portable.c blake3_neon.c
```

Some ARMv7 targets require:

```bash
-mfpu=neon-vfpv4 -mfloat-abi=hard
```

For Android/Termux ARMv7, prefer target-specific testing before documenting performance claims.

## What this fork currently adds around C build orchestration

### 1. Pointer aliasing signal

`blake3.h` defines `BLAKE3_RESTRICT` as `__restrict__` for GCC/Clang and uses it on selected public API pointer parameters. This is a compiler contract that can help optimization when callers respect non-aliasing expectations.

### 2. Include ordering as a compilation contract

The current public header includes `<stddef.h>` and `<stdint.h>`. Documentation must not claim those headers were removed.

The internal header `blake3_impl.h` then layers standard headers, the public header, internal flags, inline policy, architecture detection, NEON selection, SIMD degree, constants, helpers, and internal declarations. This order matters because preprocessor state affects which backends and visibility rules are active.

### 3. Optional TBB recursion path

`blake3_hasher_update_tbb` is compiled only under `BLAKE3_USE_TBB`. Internally, `blake3_hasher_update_base` carries a `use_tbb` boolean, and `blake3_compress_subtree_wide` delegates left/right subtree recursion to `blake3_compress_subtree_wide_join_tbb` when TBB is enabled.

`blake3_tbb.cpp` uses `oneapi::tbb::parallel_invoke` and enforces a no-exceptions build contract with a `static_assert` on `TBB_USE_EXCEPTIONS`.

### 4. CMake gates

`c/CMakeLists.txt` provides:

- `BLAKE3_USE_TBB`;
- `BLAKE3_FETCH_TBB`;
- selectable `BLAKE3_SIMD_TYPE`;
- compiler-flag variables for SSE2, SSE4.1, AVX2, AVX-512, and NEON;
- hidden symbol visibility for the C target;
- `C_EXTENSIONS OFF`;
- optional examples/tests;
- feature summary output.

### 5. Rust/C binding tests

The Rust binding crate in `c/blake3_c_rust_bindings` is documented as testing/benchmarking support, not expected production use. Its build script mirrors the C compilation router, supports optional TBB, and compiles C/C++ sources with architecture-specific flags.

## Claim gates

Use these rules in documentation and release notes:

| Claim | Status |
|---|---|
| Cryptographic core compatibility | Defensible if test vectors pass. |
| Optional TBB path exists | Defensible. |
| `BLAKE3_RESTRICT` exists | Defensible. |
| CMake flag orchestration exists | Defensible. |
| Warnings can fail CI | Defensible through workflow/build-script configuration. |
| CUDA implemented | Do not claim unless CUDA source/build/test exists. |
| Makefile optimization path exists | Do not claim unless a Makefile exists. |
| Universal speedup percentages | Do not claim without benchmark matrix. |
| Fork is globally superior to upstream | Do not claim as a blanket statement. Use axis-specific language. |

## RAFAELIA/Bitraf interpretation

Within the RAFAELIA system, BLAKE3 is best described as an integrity primitive used inside a wider custody chain:

```text
input/corpus
→ chunk normalization
→ BLAKE3/SHA3 digest
→ hashchain
→ Bitraf64 / ZIPRAF metadata
→ RAFCODE-Φ signature layer
→ GitHub/Termux reproducibility record
```

This keeps the boundary clean: BLAKE3 remains a cryptographic hash; RAFAELIA supplies orchestration, custody, and symbolic/technical metadata around it.

## Recommended next artifact

Before any strong performance claim, add a benchmark matrix with:

- CPU/SoC and core count;
- OS/kernel/Android/Termux version;
- `cc --version`, `clang --version`, `rustc -Vv`, `cmake --version`;
- flags and feature gates;
- workload sizes from small buffers to large files;
- modes: portable, NEON, x86 asm, x86 intrinsics, TBB off/on;
- repeated-run medians;
- output hashes checked against test vectors.

Until then, use precise language: this fork improves the explicitness and controllability of the build/dispatch pipeline; measured performance depends on target, compiler, flags, workload, and thermal conditions.
