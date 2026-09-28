# μ0021 — Native function dispatch, FFI, threading and provider shadows

- date: 2026-09-27
- parent_commit: `5c80eec82172a9cb573f025ac6203b5c5d9c7722`
- audited_tree: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- kind: NATIVE_FUNCTION_EXECUTION_ATLAS
- completion_state: PARTIAL
- claim_allowed: false

## μ0_START — bottom-up native question

This grain descends below workflow/script/package level and asks:

- does one public call select different machine-code functions at runtime?
- which selection depends on CPU, feature, compiler or provider state?
- where does Rust cross FFI into C/assembly?
- where can parallel execution replace serial execution?
- is explicit process/dynamic-loader execution present in the audited source?

Boundary:

`PUBLIC_FUNCTION_NAME != SELECTED_BACKEND != LINKED_OBJECT != EXECUTED_MACHINE_CODE`

## μ1_RESOLVE — execution routes

### A. C BLAKE3 CPU dispatch

`c/blake3_dispatch.c` implements a runtime CPU feature cache.

On x86 it executes CPUID/XGETBV logic and selects among linked implementations.

Representative route:

`blake3_compress_in_place`
  -> `get_cpu_features()`
  -> AVX512 if available/enabled
  -> else SSE4.1
  -> else SSE2
  -> else portable.

`blake3_hash_many` similarly selects:
- AVX512;
- AVX2;
- SSE4.1;
- SSE2;
- NEON where compiled;
- portable fallback.

`blake3_simd_degree()` exposes the selected SIMD width class.

This is runtime dispatch, but it is not a dynamic-library symbol lookup. The alternative backend functions are compile/link-time symbols selected by branches after CPU detection.

### B. preprocessor shadow

`c/blake3_impl.h` changes compiled behavior through target/compiler macros, including:

- `IS_X86`;
- `IS_X86_64`;
- `IS_X86_32`;
- `IS_AARCH64`;
- `BLAKE3_USE_NEON`;
- `BLAKE3_NO_*` controls;
- compiler-specific inline/intrinsic branches.

On AArch64, `BLAKE3_USE_NEON` is auto-enabled unless big-endian or manually overridden.

Therefore identical high-level function source can compile to materially different call routes by target and compile definitions.

### C. Rust platform dispatch

`src/platform.rs` defines `Platform::detect()` and backend-specific methods.

Detection uses `cpufeatures::new!` for x86 feature classes such as:

- AVX512F + AVX512VL;
- AVX2;
- SSE4.1;
- SSE2.

Backend methods route to portable Rust, Rust intrinsics, or unsafe FFI depending on compile configuration/platform.

Examples include unsafe FFI paths for SSE, AVX and NEON.

Thus:

`Rust public API -> Platform::detect -> feature gate -> unsafe FFI/native symbol or Rust implementation`.

### D. FFI edge

Files such as:

- `src/ffi_avx2.rs`;
- `src/ffi_neon.rs`;

declare `unsafe extern "C"` symbols and invoke linked native implementations.

The NEON module also exposes a `#[unsafe(no_mangle)] extern "C"` portable compression function used by the C side.

This is a bidirectional ABI boundary, not merely a Rust module call.

### E. parallelism shadow

`src/join.rs` provides a generic `Join` abstraction.

- `SerialJoin`: executes closures serially;
- `RayonJoin`: behind feature `rayon`, calls `rayon_core::join(oper_a, oper_b)`.

Therefore a high-level hash/update route can change its execution topology from single-threaded call order to thread-pool parallelism based on enabled feature/API.

`SAME_LOGICAL_HASH != SAME_THREAD_TOPOLOGY`

### F. OpenSSL/provider boundary

RMR crypto runtime builds `rmr-crypto-runtime` as a static library from:

- `rmr_crypto_common.c`;
- `rmr_crypto_blake3.c`;
- `rmr_crypto_openssl.c`.

CMake links it privately against:
- `BLAKE3::blake3`;
- `OpenSSL::Crypto`.

Within `rmr_crypto_openssl.c`:

- digest helpers select `EVP_md5`, `EVP_sha*`, SHA3, BLAKE2 and SM3 methods;
- XOF uses SHAKE EVP methods;
- HMAC uses `EVP_MAC_fetch(NULL, "HMAC", NULL)`;
- HKDF uses `EVP_KDF_fetch(NULL, "HKDF", NULL)`;
- key operations use EVP_PKEY for Ed25519, Ed448, X25519 and X448;
- AEAD chooses ChaCha20-Poly1305 or AES-256-GCM EVP ciphers.

The `EVP_*_fetch` calls create a provider-resolution boundary inside OpenSSL.

As established separately in μ0015, exact provider/module resolution is not proven by source identity alone.

### G. HWIF architecture branch

`rmr/CMakeLists.txt` chooses hardware-interface assembly source by `CMAKE_SYSTEM_PROCESSOR`:

- x86_64 backend;
- AArch64 backend;
- ARMv7 user backend;
- optional ARMv7 privileged backend when `RMR_ARMV7_PRIVILEGED` is enabled.

This is compile-time architecture routing.

### H. explicit loader/process search

Repository code search for the audited/default source found no occurrences for:

- `dlopen`;
- `dlsym`;
- `LoadLibrary`;
- `GetProcAddress`;
- `system(`;
- `popen(`;
- `execv`;
- `fork(`;
- `LD_PRELOAD`;
- explicit weak/alias constructor search patterns.

This is a repository-search observation, not a mathematical proof over all generated/external code.

It does not cover:
- libc/libcrypto internal loader behavior;
- external crates/actions/containers;
- generated compiler/linker code;
- package-manager scripts;
- code downloaded at CI time.

## μ2_EXECUTE_OBSERVE — shadow taxonomy

| Shadow class | State | Control source |
| --- | --- | --- |
| x86 SIMD runtime dispatch | OBSERVED | CPUID/XGETBV + compile gates |
| AArch64/NEON compile selection | OBSERVED | target + macros |
| Rust cpufeatures dispatch | OBSERVED | runtime CPU feature detection |
| Rust -> C/ASM FFI | OBSERVED | build cfg + platform |
| Serial vs Rayon topology | OBSERVED | Cargo feature/API |
| OpenSSL provider resolution | OBSERVED | libcrypto/provider state |
| RMR HWIF ISA source selection | OBSERVED | CMake target processor |
| explicit repo `dlopen/dlsym` | NOT_OBSERVED | search |
| explicit repo `system/popen/execv/fork` | NOT_OBSERVED | search |
| deliberate hidden/encoded call table | NOT_ESTABLISHED | evidence absent |

Important:

`DISPATCH != OBFUSCATION`

`INDIRECTION != MALICIOUSNESS`

These mechanisms are legitimate performance/provider mechanisms, but they must be represented in custody if the claim is about exact executed code.

## μ3_CLOSE — custody implications

To prove an exact function route, a receipt should bind at least:

1. source commit/blob hashes;
2. compiler + linker identity;
3. preprocessor/feature definitions;
4. target triple/architecture;
5. CPU feature observation;
6. linked object/archive hashes;
7. dynamic library identities where present;
8. OpenSSL version/provider configuration;
9. Cargo features such as `rayon`;
10. selected backend at runtime where observable.

Without these, a digest or test result proves the logical output under that execution, but not the exact backend path.

### R3

- F_ok: native dispatch/FFI/thread/provider routes mapped.
- F_gap: exact symbols/objects loaded in each historical job and provider module identity remain incomplete.
- F_next: enumerate binary/link boundaries (ELF/PE/Mach-O where receipts permit), shared-library dependencies and environment inheritance into downloaded code.
