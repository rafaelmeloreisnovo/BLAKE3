# μ0025 — RMR crypto runtime, OpenSSL and provider execution boundary

- date: 2026-09-27
- parent_commit: `5c3e7bce020c78b2db78c8d12553086291406569`
- audited_fork_commit: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- workflow_run: `36287272177`
- job_id: `108539902523`
- artifact_id: `10921194741`
- artifact_name: `upstream-v3-rmr-integration`
- kind: CRYPTO_RUNTIME_LIBRARY_PROVIDER_FORENSICS
- completion_state: PROVIDER_IDENTITY_PARTIAL
- claim_allowed: false

## μ0_START — retained execution evidence

The retained `upstream-v3-rmr-integration` artifact was downloaded and inspected.

It contains:
- CMake configure/build logs;
- `ctest.txt`;
- crypto registry result;
- crypto contract matrix;
- topology audit JSON/text;
- freestanding/HWIF/PAI42 receipts;
- SHA-256 manifest over retained evidence.

The historical job log additionally records installation state for OpenSSL development files.

Boundary:

`PACKAGE_INSTALLED != LIBRARY_LINKED != API_CALLED != OPENSSL_PROVIDER_RESOLVED`

## μ1_RESOLVE — build and function route

### A. OpenSSL package state

Historical job log records:

`libssl-dev is already the newest version (3.0.13-0ubuntu3.15)`.

This identifies the Ubuntu development-package version observed on the runner.

It does not by itself prove the exact `libcrypto.so` file hash or loaded provider module.

### B. RMR crypto library was compiled

Artifact `cmake-build.log` records compilation of:

- `rmr_crypto_common.c.o`;
- `rmr_crypto_blake3.c.o`;
- `rmr_crypto_openssl.c.o`;

followed by:

`Linking C static library librmr-crypto-runtime.a`

and then:

`Linking C executable rmr-crypto-runtime-selftest`.

Source CMake for the audited commit links `rmr-crypto-runtime` privately to:
- `BLAKE3::blake3`;
- `OpenSSL::Crypto`.

Therefore the source/build graph for the successful selftest is:

`selftest -> librmr-crypto-runtime.a -> {BLAKE3::blake3, OpenSSL::Crypto}`.

The retained artifact does not contain the selftest executable or a `readelf -d`/link-map for it, so the exact runtime `DT_NEEDED` entry and resolved `libcrypto` pathname/hash are not independently captured here.

### C. crypto selftest executed successfully

`ctest.txt` records:

`rmr_crypto_runtime_selftest ... Passed`

and reports:

`100% tests passed, 0 tests failed out of 4`.

The source of that selftest exercises the following RMR crypto entry points/classes:

Digest/KAT family:
- BLAKE3;
- MD5;
- SHA-1;
- SHA-224;
- SHA-256;
- SHA-384;
- SHA-512;
- SHA-512/224;
- SHA-512/256;
- SHA3-224;
- SHA3-256;
- SHA3-384;
- SHA3-512;
- BLAKE2b-512;
- BLAKE2s-256;
- SM3.

XOF:
- SHAKE128;
- SHAKE256.

MAC/KDF:
- HMAC-SHA256;
- HMAC-SHA512;
- HKDF-SHA256;
- PBKDF2-HMAC-SHA256.

Public-key:
- Ed25519 public/sign/verify;
- Ed448 public/sign/verify;
- X25519 public/shared-secret;
- X448 public/shared-secret.

AEAD:
- AES-256-GCM encrypt/decrypt;
- ChaCha20-Poly1305 encrypt/decrypt.

The selftest prints `RMR_CRYPTO_RUNTIME_KAT=PASS` only after these checks complete, but the retained non-verbose CTest output preserves only the test-level PASS, not every internal line.

### D. logical provider routing

`rmr_crypto_common.c` returns the logical provider label:

`openssl+blake3-upstream`.

This is a source-defined label and must not be confused with an OpenSSL provider-module receipt.

The implementation routes:
- BLAKE3 through the repository BLAKE3 backend;
- other listed digest/PK/AEAD operations through OpenSSL EVP APIs.

Observed OpenSSL indirection includes:
- `EVP_MAC_fetch(NULL, "HMAC", NULL)`;
- `EVP_KDF_fetch(NULL, "HKDF", NULL)`;
- EVP digest/cipher/PKEY constructors and operations.

The NULL library-context/property-query parameters permit OpenSSL's configured/default provider resolution policy to select implementations.

### E. provider identity remains explicitly unresolved

The retained contract matrix records:

`OPENSSL_CROSS_PROVIDER=TOKEN_VAZIO_PROVIDER_TOOLCHAIN`.

Historical job logs do not print:
- `openssl list -providers`;
- loaded provider module pathname/hash;
- OpenSSL property query result;
- provider name for each fetched EVP algorithm.

Thus:

`OPENSSL_API_EXECUTION = SUPPORTED_BY_BUILD_AND_SELFTEST`

`EXACT_OPENSSL_PROVIDER_MODULE = TOKEN_VAZIO`.

This is an important distinction: successful cryptographic KATs prove behavior for that execution, but not complete provider-module custody.

### F. cross architecture contract is not cross-provider runtime execution

Artifact `crypto-contract-matrix.txt` reports 7/7 compile-contract targets:

- x86_64;
- x86_32;
- armv7;
- aarch64;
- wasm32;
- riscv64 portable;
- ppc64le portable.

It explicitly records the OpenSSL cross-provider state as TOKEN_VAZIO.

Therefore:

`CROSS_COMPILE_CONTRACT_PASS != OPENSSL_PROVIDER_EXECUTION_ON_TARGET`.

### G. retained evidence hashes

Selected artifact evidence hashes:

- `cmake-build.log`:
  `92fa328a4a3fccaaa869c70539dd572b2c85ab67f234e893aaef96f40ed17023`;

- `cmake-configure.log`:
  `7b27561d2f89fdc2e5d1443444a0db258902b7eaf743f2a6391416fef21706f9`;

- `crypto-contract-matrix.txt`:
  `452d741394370fbafb324a5dfc2e1b65a5a911fc976152b3c3fee3cf8b4430c4`;

- `crypto-registry.txt`:
  `1c2b7f7e8cda77381f22aa6f6fefda3be500f3041a9b22e89a39fa46706bd1a9`;

- `ctest.txt`:
  `77fcd2f4903837a245e546ac037ff691df641670125742b0cbb794d735012da2`;

- topology audit JSON:
  `ff23de1eed5389bb4d99a16926ed8707bde4142331a0f516d7d04eb24fbe2ca5`;

- integration receipt:
  `388fb54552ac17a8bc23532aeb4a85a300c77d25edfe62e8b9d4fae6e5d37dfa`.

## μ2_EXECUTE_OBSERVE — dependency layers

For the observed host integration route:

`workflow`
 -> read-only GitHub token
 -> checkout with credentials not persisted
 -> Ubuntu package/toolchain environment
 -> CMake
 -> RMR crypto sources
 -> static RMR crypto archive
 -> BLAKE3 library + OpenSSL::Crypto
 -> selftest executable
 -> EVP/BLAKE3 calls
 -> test PASS.

Observed:
- OpenSSL development package version: YES;
- RMR OpenSSL source compiled: YES;
- RMR crypto static archive linked: YES;
- crypto runtime selftest executed: YES;
- broad algorithm-family coverage in selftest source: YES;
- exact libcrypto file hash: NOT_RETAINED;
- exact provider module: TOKEN_VAZIO;
- cross-target provider runtime: TOKEN_VAZIO;
- evidence of intentionally hidden provider replacement: NOT_ESTABLISHED.

The source-level EVP provider abstraction is a legitimate indirection layer. It is a custody gap only when a claim requires the exact implementation/module that ran.

## μ3_CLOSE — result and next route

### Result

The RMR crypto runtime is not a fully freestanding implementation path: for the host integration route, most algorithms intentionally pass through OpenSSL's EVP abstraction, while BLAKE3 remains a repository backend.

The test evidence establishes functional execution, but the provider identity beneath EVP remains unbound.

This yields the chain:

`RMR API -> algorithm selector -> OpenSSL EVP -> provider resolution -> implementation`

where the final provider-resolution node is currently:

`TOKEN_VAZIO_PROVIDER_TOOLCHAIN`.

### Required provider receipt

A future hermetic run should record:
- `openssl version -a`;
- `openssl list -providers -verbose`;
- `openssl list -digest-algorithms/-cipher-algorithms` where relevant;
- resolved `libcrypto` path + SHA-256;
- provider module paths + SHA-256;
- `OPENSSL_CONF`, `OPENSSL_MODULES` state without exposing secrets;
- algorithm/provider mapping for EVP fetches where obtainable;
- final executable `DT_NEEDED`, RUNPATH/RPATH;
- process maps or loader audit evidence when exact runtime linkage is a claim.

### R3

- F_ok: host crypto build, link graph, function-family coverage and selftest execution reconstructed.
- F_gap: exact libcrypto/provider module identity remains TOKEN_VAZIO.
- F_next: materialize a canonical route atlas joining source, workflow, privilege, resolver, native dispatch, provider, binary and retained evidence nodes.
