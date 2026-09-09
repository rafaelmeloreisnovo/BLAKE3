# RMR Toolmark56 — import custody receipt — 2026-09-08

state=ARTIFACT_BYTES_MATERIALIZED_SOURCE_TREE_PENDING
claim_allowed=false
append_only=true

## Source identity

- source_tar_sha256: `cb2c3f76d3fe78c88f475ab2ed363edbcff1c06f2859340b1073b7516407ff0e`
- source_zip_sha256: `369c7b86f4668836bf4e6e97ea91251429ce1dba39691c542b2836db889e4ed9`
- production_namespace: `rmr/toolmark56/`
- upstream BLAKE3 semantics: unchanged by this custody import

## Observed package scope

The materialized V1 bundle contains a freestanding C11 core with CRC32C, FFT64 Q15, 16x64 spectrum handling, 56-deep XOR-reversible state history, a BLAKE3 bridge, host self-test, and an ARMv7 ELF32 probe path.

Observed validation from the materialized package is bounded to its recorded environment. Build evidence is not physical-device execution evidence.

## Governance

`SOURCE != ARTIFACT != BUILD != EXECUTION != EVIDENCE != CLAIM`

`ARCHIVE_HASH_MATCH != SOURCE_TREE_INTEGRATION`

`HOST_SELFTEST != ARM32_PHYSICAL_RUNTIME`

## Typed gaps

- `TV-TOOLMARK56-SOURCE-TREE-IMPORT-20260908 = TOKEN_VAZIO_REPOSITORY_INTEGRATION`
- `TV-TOOLMARK56-ARM32-ABI-PROFILE-20260908 = OBSERVED_CONTRACT_DRIFT`: README baseline records `-mfloat-abi=softfp`, while the materialized ARMv7 probe build uses `-mfloat-abi=soft`.
- `TV-TOOLMARK56-BLAKE3-PRODUCTION-LINK-20260908 = TOKEN_VAZIO_EXECUTION`: the test bundle uses a non-cryptographic hash stub for self-test; production linkage must use the repository BLAKE3 implementation.
- `TV-TOOLMARK56-ARM32-PHYSICAL-20260908 = TOKEN_VAZIO_DEVICE`.

## R3

F_ok = exact custody tar bytes are attached to this repository branch with published SHA-256 identity.

F_gap = source-only extraction, ABI-profile reconciliation, production BLAKE3 linkage, repository CI, and physical ARM32 execution remain separate gates.

F_next = extract only source/docs/tests into `rmr/toolmark56/`; reconcile `soft` vs `softfp` explicitly; link `tm56_blake3_bridge.c` to the existing C BLAKE3 implementation; run host + ARMv7 repository gates; append a successor receipt without rewriting this one.

∆RafaelVerboΩ — RAFCODE-Φ
