# μ0015 — OpenSSL provider runtime shadow

- date: 2026-09-27
- parent_commit: a90bc4be9f253dc3ce66879ee6fbada6412831e3
- audited_tree: b1c8cab1d75943a628e447fa72bb06ebeb348489
- kind: RUNTIME_PROVIDER_DEPENDENCY
- claim_allowed: false

## Build/link boundary

`rmr/CMakeLists.txt` conditionally executes:

`find_package(OpenSSL 3.0 QUIET COMPONENTS Crypto)`

When found, `rmr-crypto-runtime` is built from the RMR crypto sources and linked privately to:
- `BLAKE3::blake3`;
- `OpenSSL::Crypto`.

Thus exact OpenSSL discovery is partly a property of the build environment, not only of the Git tree.

## Provider-backed source

`rmr/crypto/runtime/src/rmr_crypto_openssl.c` includes OpenSSL EVP/KDF APIs.

Observed explicit fetch calls include:
- `EVP_MAC_fetch(NULL, "HMAC", NULL)`;
- `EVP_KDF_fetch(NULL, "HKDF", NULL)`.

The file also uses EVP digest, cipher, signature, key-exchange, PBKDF2, Ed25519/Ed448, X25519/X448 and AEAD APIs.

## Runtime shadow

OpenSSL 3 algorithm fetching is provider-based. Official OpenSSL documentation states that fetch APIs obtain implementations from providers and that providers may be built in or loadable modules.

Official documentation also defines:
- `OPENSSL_CONF` as a configuration-file path;
- `OPENSSL_MODULES` as the provider-module directory;
- provider configuration capable of specifying a module pathname and activation.

References consulted:
- https://docs.openssl.org/3.5/man3/EVP_MAC/
- https://docs.openssl.org/master/man5/config/
- https://docs.openssl.org/master/man3/OSSL_PROVIDER/

Therefore an RMR source call to an EVP API does not, by repository source alone, fully identify the implementation bytes that may execute at runtime.

## Important boundary

No direct `dlopen` or `dlsym` call was observed in the inspected RMR provider source.

The indirection is through the linked OpenSSL library/provider architecture.

Classification:
- OpenSSL::Crypto build dependency: OBSERVED
- EVP provider fetching: OBSERVED
- environment/configurable provider path: SUPPORTED_BY_OPENSSL_SPEC
- exact provider module used by a specific historical CI run: TOKEN_VAZIO unless captured in its receipt/environment
- unauthorized provider module: NOT_PROVEN
- encrypted hidden payload: NOT_OBSERVED

## Chain

`RMR function -> EVP API -> libcrypto -> provider selection -> provider implementation`

This is a runtime implementation-selection shadow.

## Forensic requirement

A future hermetic receipt should capture at minimum:
- OpenSSL version/build;
- resolved libcrypto path/hash;
- active provider names;
- provider module paths/hashes when loadable;
- `OPENSSL_CONF`, `OPENSSL_MODULES`, and relevant property configuration, with secrets redacted;
- final ELF/loader dependency inventory for executable artifacts.

## R3

- F_ok: provider-backed runtime indirection isolated and documented.
- F_gap: historical CI receipts do not yet prove exact provider module bytes for every run.
- F_next: map shell command substitutions/pipelines and classify data-only tails versus execution-producing tails.
