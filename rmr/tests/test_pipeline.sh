#!/usr/bin/env bash
set -euo pipefail

export RMR_DETERMINISTIC=1

PAI_BIN="${PAI_BIN:-./pai}"
OUT_BASE="${OUT_BASE:-out_rmr_pipeline}"

rm -rf "$OUT_BASE"
mkdir -p "$OUT_BASE"

"$PAI_BIN" validate \
  --alpha 0.25 \
  --attractors 42 \
  --rows 10 \
  --cols 10 \
  --dr 7 \
  --dc 9 \
  --unique 42 \
  --transitions 64 \
  --len 128

"$PAI_BIN" scan \
  --base rmr \
  --out "$OUT_BASE/scan" \
  --max-size 1048576

"$PAI_BIN" sign \
  --base rmr \
  --scan "$OUT_BASE/scan" \
  --out "$OUT_BASE/scan"

"$PAI_BIN" bench \
  --repeat 3 \
  --out "$OUT_BASE/bench_a" \
  --metrics-store "$OUT_BASE/metrics" \
  --new-session \
  -- "$PAI_BIN" validate

"$PAI_BIN" benchdiff \
  --a "$OUT_BASE/bench_a/bench.tsv" \
  --b "$OUT_BASE/bench_a/bench.tsv" \
  --out "$OUT_BASE/diff" \
  --threshold 5

echo "[OK] RMR pipeline canonical smoke test passed"
