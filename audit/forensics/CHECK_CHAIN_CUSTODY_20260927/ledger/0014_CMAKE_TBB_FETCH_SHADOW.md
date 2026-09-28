# μ0014 — CMake/TBB package and fetch shadow

- date: 2026-09-27
- parent_commit: 85e7ad92ea93d98be1138222526d50e7c1d6f83a
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: CMAKE_TRANSITIVE_DEPENDENCY
- claim_allowed: false

## C library entry point

`c/CMakeLists.txt` declares:
- `BLAKE3_USE_TBB` default OFF;
- `BLAKE3_FETCH_TBB` default OFF.

It also executes:

`add_subdirectory(dependencies)`

This creates a child configuration edge not visible from a parent `cmake -S c ...` command alone.

## Dependency child

`c/dependencies/CMakeLists.txt` conditionally enters `tbb` when `BLAKE3_USE_TBB` is enabled.

`c/dependencies/tbb/CMakeLists.txt` performs:
1. `find_package(TBB 2021.11.0 QUIET)`;
2. includes CMake `FetchContent` on supported CMake versions;
3. when TBB is absent and `BLAKE3_FETCH_TBB` is true, declares external oneTBB;
4. fetches from `https://github.com/uxlfoundation/oneTBB`;
5. selects Git commit `0c0ff192a2304e114bc9e6557582dfba101360ff` (commented as v2022.0.0);
6. invokes `FetchContent_MakeAvailable(TBB)`.

Classification:
- external network source: OBSERVED
- upstream repository: OBSERVED
- intended Git object pinned by full SHA: YES
- mutable branch/tag used for TBB object: NO in inspected declaration
- fetched bytes stored in this repository tree: NO

## Workflow activation

Base CI contains a CMake configuration path that sets:

`-DBLAKE3_FETCH_TBB=${{ matrix.os == 'windows-latest' && 'YES' || 'NO' }}`

Combined with TBB-enabled matrix configurations, this can activate the external FetchContent path on Windows when TBB is not otherwise found.

Thus:

`workflow -> cmake -> c/CMakeLists -> dependencies -> tbb/CMakeLists -> FetchContent -> GitHub oneTBB@SHA`

## RMR isolation difference

`rmr/CMakeLists.txt` explicitly forces:

`set(BLAKE3_FETCH_TBB OFF CACHE BOOL "" FORCE)`

before adding the repository C library as a subdirectory.

It also has `RMR_BLAKE3_USE_TBB` default OFF and separately performs `find_package(OpenSSL 3.0 QUIET COMPONENTS Crypto)` for the provider-backed crypto runtime path.

Therefore:
- base/upstream-style CMake CI can have a TBB network-fetch edge;
- the inspected RMR CMake entry explicitly disables BLAKE3's TBB FetchContent edge;
- OpenSSL discovery remains an environment/system dependency in the RMR runtime path.

## Shadow classification

A visible command such as:

`cmake -S c -B c/build ...`

may transitively cause package discovery and, under the enabling conditions above, a network Git fetch.

This is build-system indirection, not evidence of encoded or encrypted commands.

## R3

- F_ok: CMake child directories, TBB discovery, external FetchContent and RMR OFF boundary mapped.
- F_gap: system dynamic-link/runtime dependency surfaces and command-level process fan-out remain.
- F_next: map ELF/shared-library/provider dependencies and distinguish build-time from runtime linkage.
