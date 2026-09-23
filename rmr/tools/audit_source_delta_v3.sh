#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/upstream_validation/common.sh"
for t in git python3 sha256sum; do rmr_need "$t"; done
WORK="${WORK_ROOT:-$ROOT/.rmr-work/upstream-v3-source}"
OUT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/upstream-v3-source}"
OFFICIAL_ROOT="$WORK/official"
rm -rf "$OUT"; mkdir -p "$WORK" "$OUT"
OFFICIAL_COMMIT="$(rmr_checkout_official "$OFFICIAL_ROOT")"
rmr_write_common_environment "$OUT/environment.txt" "$OFFICIAL_COMMIT"
python3 "$ROOT/upstream_validation/audit_source_delta_v3.py"   --official "$OFFICIAL_ROOT" --fork "$RMR_REPO_ROOT"   --out "$OUT/source-delta.json" | tee "$OUT/analysis.txt"
sha256sum "$OUT"/*.txt "$OUT"/*.json >"$OUT/SHA256SUMS.txt"
