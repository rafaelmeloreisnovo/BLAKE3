#!/usr/bin/env bash
set -euo pipefail

export RMR_DETERMINISTIC=1

if [[ -n "${PAI_BIN:-}" ]]; then
  # shellcheck disable=SC2206
  PAI_CMD=(${PAI_BIN})
else
  PAI_CMD=(./pai run --mode cli)
fi
OUT_BASE="${OUT_BASE:-out_rmr_pipeline}"

rm -rf "$OUT_BASE"
mkdir -p "$OUT_BASE"

"${PAI_CMD[@]}" validate --alpha 0.25 --attractors 42 --rows 10 --cols 10 --dr 7 --dc 9 --unique 42 --transitions 64 --len 128
"${PAI_CMD[@]}" scan --base rmr --out "$OUT_BASE/scan" --max-size 1048576 --hash sha256
"${PAI_CMD[@]}" sign --base rmr --scan "$OUT_BASE/scan" --out "$OUT_BASE/scan" --self ./pai
"${PAI_CMD[@]}" bench --repeat 3 --out "$OUT_BASE/bench_a" --metrics-store "$OUT_BASE/metrics" --new-session -- hash --file README.md
"${PAI_CMD[@]}" benchdiff --a "$OUT_BASE/bench_a/bench.tsv" --b "$OUT_BASE/bench_a/bench.tsv" --out "$OUT_BASE/diff" --threshold 5

echo "[OK] RMR pipeline canonical smoke test passed"
