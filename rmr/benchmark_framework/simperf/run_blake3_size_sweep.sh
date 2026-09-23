#!/usr/bin/env sh
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -eu

OFFICIAL_BIN=${OFFICIAL_BIN:?set OFFICIAL_BIN to official BLAKE3 size harness}
RMR_BIN=${RMR_BIN:?set RMR_BIN to RMR/fork BLAKE3 size harness}
OUT=${OUT:-/tmp/rmr_blake3_size_sweep}
SIZES=${SIZES:-"64 256 1024 4096 16384 65536 1048576 16777216"}
ITER_SMALL=${ITER_SMALL:-20000}
ITER_LARGE=${ITER_LARGE:-64}

mkdir -p "$OUT"
printf 'algorithm,size_bytes,ns_per_op\n' > "$OUT/official.csv"
printf 'algorithm,size_bytes,ns_per_op\n' > "$OUT/rmr.csv"

run_one() {
  bin=$1
  mode=$2
  size=$3
  iterations=$4
  line=$("$bin" "$mode" "$size" "$iterations")
  ns=$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $6}')
  if [ -z "$ns" ]; then
    echo "invalid SIZE_RESULT from $bin" >&2
    exit 1
  fi
  printf 'blake3,%s,%s\n' "$size" "$ns"
}

for size in $SIZES; do
  if [ "$size" -le 65536 ]; then
    iters=$ITER_SMALL
  else
    iters=$ITER_LARGE
  fi
  run_one "$OFFICIAL_BIN" official "$size" "$iters" >> "$OUT/official.csv"
  run_one "$RMR_BIN" rmr "$size" "$iters" >> "$OUT/rmr.csv"
done

echo "RMR_SIZE_SWEEP=PASS out=$OUT"
echo "SIMULATION_INPUT_ONLY=true"
