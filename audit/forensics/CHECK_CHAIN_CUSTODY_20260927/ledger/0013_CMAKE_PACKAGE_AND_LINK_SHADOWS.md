# μ0013 — CMake package, fetch, and link shadows

- date: 2026-09-27
- parent_commit: 91abdfc82aa3eaf56cbc937fbf3cbeea431397c2
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: CMAKE_RESOLUTION_GRAPH
- claim_allowed: false

## oneTBB resolution chain

`c/CMakeLists.txt` defines:
- `BLAKE3_USE_TBB=OFF` by default;
- `BLAKE3_FETCH_TBB=OFF` by default;
- `add_subdirectory(dependencies)`.

`c/dependencies/CMakeLists.txt` enters the TBB subtree only when `BLAKE3_USE_TBB` is enabled.

`c/dependencies/tbb/CMakeLists.txt` then:
1. calls `find_package(TBB 2021.11.0 QUIET)`;
2. if TBB is absent and `BLAKE3_FETCH_TBB` is enabled, uses CMake `FetchContent`;
3. fetches `https://github.com/uxlfoundation/oneTBB`;
4. pins `GIT_TAG 0c0ff192a2304e114bc9e6557582dfba101360ff` (commented as v2022.0.0);
5. makes the fetched project available.

Classification:
- system TBB discovery: SHADOW / host-dependent;
- FetchContent repository: external;
- FetchContent Git object: EXTERNAL_PINNED_BY_COMMIT;
- default fetch path: OFF.

## Important CI override

The upstream/base CI current CMake job uses:
`-DBLAKE3_FETCH_TBB=${{ matrix.os == 'windows-latest' && 'YES' || 'NO' }}`

Therefore the fetch path that is OFF by source default can become ON in a Windows CI matrix context.

This is a configuration-dependent tail:
`ci matrix -> CMake option -> dependencies/tbb -> FetchContent -> pinned oneTBB commit`.

## RMR hardening boundary

`rmr/CMakeLists.txt` explicitly sets:
- `BLAKE3_USE_TBB` from `RMR_BLAKE3_USE_TBB`;
- `BLAKE3_FETCH_TBB OFF CACHE BOOL "" FORCE`.

Thus the RMR top-level CMake path explicitly disables automatic TBB fetch even when optional TBB use is selected. This is materially different from the base C CI path.

## OpenSSL shadow

`rmr/CMakeLists.txt`:
- enables `RMR_BUILD_CRYPTO_RUNTIME` by default;
- calls `find_package(OpenSSL 3.0 QUIET COMPONENTS Crypto)`;
- builds and links `rmr-crypto-runtime` against `OpenSSL::Crypto` only when found;
- otherwise prints a disable message.

Therefore:

`same Git commit + different host OpenSSL state -> different built target set`.

This is a host-resolution shadow, not hidden code.

## Architecture/compiler selection

CMake source selection depends on:
- `CMAKE_C_COMPILER_ID`;
- `CMAKE_CXX_COMPILER_ID`;
- `CMAKE_SYSTEM_PROCESSOR`;
- `CMAKE_SIZEOF_VOID_P`;
- MSVC architecture metadata;
- build options.

RMR HWIF source selection additionally chooses:
- x86_64 assembly;
- AArch64 assembly;
- ARMv7 user assembly;
- optional ARMv7 privileged assembly when `RMR_ARMV7_PRIVILEGED` is enabled.

Therefore compiler/host state is part of the effective source graph.

## Link shadows

Observed CMake links include:
- `BLAKE3::blake3`;
- system math library `m` on non-MSVC paths;
- optional `TBB::tbb`;
- optional `OpenSSL::Crypto`.

A source-level function dependency can therefore acquire external ABI/runtime dependencies through target-level linkage even when the individual C function body contains no external call declaration.

## Obfuscation classification

- generated/conditional build graph: OBSERVED
- external package discovery: OBSERVED
- pinned FetchContent tail: OBSERVED
- RMR automatic TBB fetch: DISABLED_BY_FORCE
- host OpenSSL-dependent target presence: OBSERVED
- encoded/encrypted CMake command: NOT_OBSERVED
- deliberate concealment: NOT_ESTABLISHED

## R3

- F_ok: CMake discovery, FetchContent, link, and architecture shadows mapped.
- F_gap: complete shell process graph and Python child-process graph remain.
- F_next: enumerate shell CALL/IMPORT/RESOLVE edges across all repository shell scripts.
