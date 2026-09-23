#!/usr/bin/env sh
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-clang}
ARCH=$(uname -m 2>/dev/null || echo unknown)
TS=$(date -u +%Y%m%dT%H%M%SZ 2>/dev/null || echo no_utc_clock)
OUT=${OUT:-"$ROOT/reports/device_perf/$TS"}
IOPS_BLOCK_SIZE=${IOPS_BLOCK_SIZE:-4096}
IOPS_FILE_SIZE=${IOPS_FILE_SIZE:-16777216}
IOPS_OPS=${IOPS_OPS:-4096}
IOPS_CACHE=${IOPS_CACHE:-os-default}
IOPS_SYNC=${IOPS_SYNC:-end}

mkdir -p "$OUT"

HWIF_ASM=""
HWIF_PROFILE="unsupported"
case "$ARCH" in
  x86_64|amd64)
    HWIF_ASM="$ROOT/hwif/asm/x86_64/rmr_hwif_backend.S"
    HWIF_PROFILE="x86_64_user"
    ;;
  aarch64|arm64)
    HWIF_ASM="$ROOT/hwif/asm/aarch64/rmr_hwif_backend.S"
    HWIF_PROFILE="aarch64_current"
    ;;
  armv7l|armv8l|arm)
    HWIF_ASM="$ROOT/hwif/asm/armv7/rmr_hwif_user.S"
    HWIF_PROFILE="armv7_linux_user"
    ;;
esac

{
  echo "schema=RMR-DEVICE-PERF-RECEIPT-V1"
  echo "timestamp_utc=$TS"
  echo "arch=$ARCH"
  echo "hwif_profile=$HWIF_PROFILE"
  echo "claim_allowed=false"
  echo "privileged_armv7=false"
  echo "queue_depth=1"
  echo "direct_io=false"
  if command -v git >/dev/null 2>&1; then
    echo "git_head=$(git -C "$ROOT/.." rev-parse HEAD 2>/dev/null || echo TOKEN_VAZIO)"
  else
    echo "git_head=TOKEN_VAZIO"
  fi
  echo "compiler=$("$CC" --version 2>/dev/null | sed -n '1p')"
  uname -a 2>/dev/null | sed 's/^/uname=/'
} > "$OUT/environment.txt"

if [ -n "$HWIF_ASM" ]; then
  "$CC" -std=c11 -O2 -Wall -Wextra -Wpedantic \
    -I"$ROOT/include" -I"$ROOT/hwif/include" \
    "$ROOT/hwif/rmr_hwif.c" \
    "$HWIF_ASM" \
    "$ROOT/hwif/tests/rmr_hwif_selftest.c" \
    -o "$OUT/rmr_hwif_selftest"

  "$OUT/rmr_hwif_selftest"
  echo "hwif_selftest=PASS" >> "$OUT/environment.txt"
else
  echo "hwif_selftest=TOKEN_VAZIO_UNSUPPORTED_ARCH" >> "$OUT/environment.txt"
fi

"$CC" -std=c11 -O2 -Wall -Wextra -Wpedantic -Wformat=2 \
  "$ROOT/benchmark_framework/core/iops_bench.c" \
  -o "$OUT/rmr_iops_bench"

"$OUT/rmr_iops_bench" \
  --file "$OUT/iops_test.bin" \
  --mode write \
  --pattern seq \
  --block-size "$IOPS_BLOCK_SIZE" \
  --file-size "$IOPS_FILE_SIZE" \
  --ops "$IOPS_OPS" \
  --sync "$IOPS_SYNC" \
  --cache "$IOPS_CACHE" \
  --queue-depth 1 \
  > "$OUT/iops_write.json"

"$OUT/rmr_iops_bench" \
  --file "$OUT/iops_test.bin" \
  --mode read \
  --pattern random \
  --block-size "$IOPS_BLOCK_SIZE" \
  --file-size "$IOPS_FILE_SIZE" \
  --ops "$IOPS_OPS" \
  --sync none \
  --cache "$IOPS_CACHE" \
  --queue-depth 1 \
  > "$OUT/iops_read.json"

if command -v sha256sum >/dev/null 2>&1; then
  (
    cd "$OUT"
    sha256sum environment.txt rmr_iops_bench iops_write.json iops_read.json > SHA256SUMS.txt
    if [ -f rmr_hwif_selftest ]; then
      sha256sum rmr_hwif_selftest >> SHA256SUMS.txt
    fi
  )
fi

rm -f "$OUT/iops_test.bin"

echo "RMR_DEVICE_RECEIPT=$OUT"
echo "PHYSICAL_EXECUTION=OBSERVED_BY_THIS_SCRIPT_ONLY_IF_EXIT_0"
echo "CROSS_DEVICE_CLAIM_ALLOWED=false"
