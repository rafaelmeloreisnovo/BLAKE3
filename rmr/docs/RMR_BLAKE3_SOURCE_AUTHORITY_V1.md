<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR BLAKE3 Source Authority Contract V1

Normal RMR builds use `rmr/CMakeLists.txt -> add_subdirectory(../c)`, therefore
they execute the fork's current `c/`.

Official comparison harnesses fetch
`https://github.com/BLAKE3-team/BLAKE3.git` at pinned
`6aab490a26124663329dfd3961b8469f8fdb158b`, while the fork side remains the
current checkout. Receipts record both commits.

Enforced surfaces:
- `rmr/upstream_validation/common.sh`
- `rmr/tools/orchestrate_blake3_compare.sh`
- `rmr/tools/orchestrate_blake3_compare_v2.sh`
- `rmr/CMakeLists.txt`
- `rmr/crypto/runtime/src/rmr_crypto_blake3.c`
- `.github/workflows/rmr-upstream-comprehensive-v3.yml`

`validate_source_authority.py` fails closed on official-repo drift, pinned-ref
divergence, or fork-runtime source drift.
