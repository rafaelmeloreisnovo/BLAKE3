#!/usr/bin/env bash
set -euo pipefail

REPORT_DIR="rmr/reports"
OUT_BASE="out_rmr_audit"
mkdir -p "$REPORT_DIR"

exec > >(tee "$REPORT_DIR/RMR_BUILD_LOG.txt") 2>&1

echo "[STEP] build"
./rmr/tools/build_pai.sh

echo "[STEP] pipeline test"
PAI_BIN="./pai run --mode cli" ./rmr/tests/test_pipeline.sh | tee "$REPORT_DIR/RMR_TEST_LOG.txt"

echo "[STEP] scan/sign/bench/benchdiff"
rm -rf "$OUT_BASE"
mkdir -p "$OUT_BASE"
./pai run --mode cli scan --base rmr --out "$OUT_BASE/scan" --hash sha256
./pai run --mode cli sign --base rmr --scan "$OUT_BASE/scan" --out "$OUT_BASE/scan" --self ./pai
./pai run --mode cli bench --repeat 3 --out "$OUT_BASE/bench_a" --metrics-store "$OUT_BASE/metrics" --new-session -- hash --file README.md
./pai run --mode cli benchdiff --a "$OUT_BASE/bench_a/bench.tsv" --b "$OUT_BASE/bench_a/bench.tsv" --out "$OUT_BASE/diff" --threshold 5

sha256sum ./pai \
  "$OUT_BASE/scan/manifest.tsv" \
  "$OUT_BASE/scan/linear_manifest_root.txt" \
  "$OUT_BASE/scan/merkle_root.txt" \
  "$OUT_BASE/scan/SIGNATURE.txt" \
  "$OUT_BASE/bench_a/bench.tsv" \
  "$OUT_BASE/diff/benchdiff_report.txt" \
  "$OUT_BASE/diff/benchdiff_report.json" > "$REPORT_DIR/RMR_ARTIFACTS_SHA256.txt"

cat > "$REPORT_DIR/RMR_PIPELINE_REPORT.md" <<MD
# RMR Pipeline Report

- Build: \
  - Binary `pai` compiled via `rmr/tools/build_pai.sh`.
- Validation flow executed:
  - `scan` (sha256)
  - `sign` (PAI SIGNATURE v2)
  - `bench`
  - `benchdiff` (mean + p95 threshold)
- Logs:
  - Build/Test logs em `rmr/reports/`.
- Artifact integrity:
  - SHA-256 list in `RMR_ARTIFACTS_SHA256.txt`.
MD

echo "[OK] full audit complete"
