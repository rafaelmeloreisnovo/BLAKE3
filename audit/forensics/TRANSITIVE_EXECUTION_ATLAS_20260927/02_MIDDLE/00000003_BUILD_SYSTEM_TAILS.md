# μTRACE 0x00000003 — MIDDLE / build-system tails and environment shadows

parent_mu=0x00000002
phase=02_MIDDLE
source_ref=blobs@5365b389919881f91be4a407ab5b2d094bc1a65c
evidence_state=OBSERVED
claim_allowed=false

## Rust build-script tails
### root build.rs
Cargo automatically executes root build.rs. Observed effects include:
- reads TARGET/HOST/CC/CFLAGS/CARGO_CFG_TARGET_* and feature variables
- invokes cc::Build
- compiles architecture-dependent C/ASM
- may mutate CFLAGS by appending -fno-lto
- emits cfg/link/build directives

Classification: SHADOW_CANDIDATE relative to a parent line such as `cargo build`; OBSERVED build behavior; no concealment intent inferred.

### c/blake3_c_rust_bindings/build.rs
Observed:
- cc::Build C/C++ compilation
- environment-controlled BLAKE3_C_DIR_OVERRIDE changes source directory
- environment CC/CFLAGS affect compiler behavior
- links TBB when feature=tbb
- removes generated asm files in one Windows path
- walks parent tree to emit rerun-if-changed directives

### tools/compiler_version/build.rs
Observed:
- cc::Build::get_compiler()
- exports selected compiler path as COMPILER_PATH to Rust compilation.

## CMake external/library edges
### c/dependencies/tbb/CMakeLists.txt
When BLAKE3_USE_TBB and BLAKE3_FETCH_TBB are enabled and TBB is absent:
- CMake FetchContent is activated
- external repository: uxlfoundation/oneTBB
- GIT_TAG=0c0ff192a2304e114bc9e6557582dfba101360ff
- shallow fetch
The external revision is SHA-pinned. Network activity remains an EXTERNAL edge.

### rmr/CMakeLists.txt
RMR explicitly forces BLAKE3_FETCH_TBB=OFF for its add_subdirectory path.
RMR crypto runtime conditionally discovers system OpenSSL >=3.0 Crypto and links OpenSSL::Crypto.
On UNIX, pai links system math library m.
Architecture controls selection of x86_64/aarch64/armv7 assembly HWIF sources.

## Important distinction
RMR_CMAKE_TBB_FETCH=DISABLED_OBSERVED
UPSTREAM_CMAKE_TBB_FETCH_CAPABILITY=OBSERVED_CONDITIONAL
OPENSSL_SYSTEM_DEPENDENCY=OBSERVED_CONDITIONAL
SYSTEM_LIBM_DEPENDENCY=OBSERVED_CONDITIONAL
ENVIRONMENT_CONTROLLED_BUILD_EDGES=OBSERVED
OBFUSCATED_COMMANDS=NOT_PROVEN

mu_record=0x00000003|0x00000002|02_MIDDLE|build.rs+CMake|expand compiler/library/network tails|OBSERVED|full Cargo/action/package dependency closure pending|inspect workflow supply-chain edges
