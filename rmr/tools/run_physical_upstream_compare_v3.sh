#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# Device-bound official-upstream vs fork benchmark.
# This script produces physical evidence only when it actually exits 0 on the
# target device. Cross-compilation, CI and SIMPERF must not be substituted.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-6aab490a26124663329dfd3961b8469f8fdb158b}"
CC_BIN="${CC:-clang}"
CXX_BIN="${CXX:-clang++}"
ROUNDS="${ROUNDS:-7}"
TARGET_MIB="${TARGET_MIB:-96}"
SIZES="${SIZES:-64 256 1024 4096 65536 1048576 16777216}"
ARCH="$(uname -m 2>/dev/null || echo unknown)"
STAMP="$(date -u +%Y%m%dT%H%M%SZ 2>/dev/null || echo no-utc-clock)"
OUT="${OUT:-$ROOT/rmr/reports/device_upstream_compare/$STAMP}"
WORK="${WORK:-$ROOT/.rmr-work/device-upstream-v3}"
OFFICIAL_ROOT="$WORK/official"
OFFICIAL_BUILD="$WORK/build-official"
FORK_BUILD="$WORK/build-fork"

mkdir -p "$OUT" "$WORK"
rm -rf "$OFFICIAL_BUILD" "$FORK_BUILD"

snapshot_runtime() {
  local label="$1"
  local file="$OUT/telemetry-$label.txt"
  {
    echo "label=$label"
    echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ 2>/dev/null || true)"
    echo "arch=$ARCH"
    if command -v nproc >/dev/null 2>&1; then
      echo "logical_cpus=$(nproc 2>/dev/null || true)"
    elif command -v getconf >/dev/null 2>&1; then
      echo "logical_cpus=$(getconf _NPROCESSORS_ONLN 2>/dev/null || true)"
    fi
    if [ -r /proc/meminfo ]; then
      grep -E '^(MemTotal|MemFree|MemAvailable):' /proc/meminfo || true
    fi
    for cpu in /sys/devices/system/cpu/cpu[0-9]*; do
      [ -d "$cpu" ] || continue
      name=$(basename "$cpu")
      for field in scaling_cur_freq scaling_min_freq scaling_max_freq scaling_governor cpuinfo_cur_freq cpuinfo_max_freq; do
        p="$cpu/cpufreq/$field"
        [ -r "$p" ] && echo "$name.$field=$(cat "$p" 2>/dev/null || true)"
      done
    done
    for zone in /sys/class/thermal/thermal_zone*; do
      [ -d "$zone" ] || continue
      z=$(basename "$zone")
      type=""
      temp=""
      [ -r "$zone/type" ] && type=$(cat "$zone/type" 2>/dev/null || true)
      [ -r "$zone/temp" ] && temp=$(cat "$zone/temp" 2>/dev/null || true)
      echo "$z.type=$type"
      echo "$z.temp=$temp"
    done
  } >"$file"
}

snapshot_runtime before

for t in git cmake "$CC_BIN" "$CXX_BIN" python3 sha256sum; do
  command -v "$t" >/dev/null 2>&1 || {
    echo "missing_command=$t" >&2
    exit 127
  }
done

if [ ! -d "$OFFICIAL_ROOT/.git" ]; then
  git clone --filter=blob:none --no-tags "$OFFICIAL_REPO" "$OFFICIAL_ROOT"
fi
git -C "$OFFICIAL_ROOT" fetch --depth 1 origin "$OFFICIAL_REF"
git -C "$OFFICIAL_ROOT" checkout --detach FETCH_HEAD

SIMD_ARGS=()
CFLAGS="-O3 -DNDEBUG"
case "$ARCH" in
  armv7l|armv8l|arm)
    SIMD_ARGS=(
      "-DBLAKE3_SIMD_TYPE=neon-intrinsics"
      "-DBLAKE3_CFLAGS_NEON=-mfpu=neon-vfpv4"
    )
    CFLAGS="$CFLAGS -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=softfp"
    PROFILE="armv7-neon"
    ;;
  aarch64|arm64)
    SIMD_ARGS=("-DBLAKE3_SIMD_TYPE=neon-intrinsics")
    CFLAGS="$CFLAGS -march=armv8-a"
    PROFILE="aarch64-neon"
    ;;
  x86_64|amd64)
    SIMD_ARGS=("-DBLAKE3_SIMD_TYPE=amd64-asm")
    PROFILE="x86_64-auto"
    ;;
  *)
    SIMD_ARGS=("-DBLAKE3_SIMD_TYPE=none")
    PROFILE="portable-$ARCH"
    ;;
esac

GEN=()
if command -v ninja >/dev/null 2>&1; then
  GEN=(-G Ninja)
fi

configure_build() {
  local src="$1"
  local build="$2"
  cmake -S "$src/c" -B "$build" "${GEN[@]}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER="$CC_BIN" \
    -DCMAKE_CXX_COMPILER="$CXX_BIN" \
    -DCMAKE_C_FLAGS="$CFLAGS" \
    -DCMAKE_COMPILE_WARNING_AS_ERROR=ON \
    -DBLAKE3_USE_TBB=OFF \
    "${SIMD_ARGS[@]}"
  cmake --build "$build" --parallel 2
}

configure_build "$OFFICIAL_ROOT" "$OFFICIAL_BUILD" >"$OUT/official-build.log" 2>&1
configure_build "$ROOT" "$FORK_BUILD" >"$OUT/fork-build.log" 2>&1

OFFICIAL_LIB="$(find "$OFFICIAL_BUILD" -type f -name libblake3.a -print -quit)"
FORK_LIB="$(find "$FORK_BUILD" -type f -name libblake3.a -print -quit)"
[ -n "$OFFICIAL_LIB" ] && [ -n "$FORK_LIB" ]

"$CC_BIN" -O3 -std=c11 -I"$OFFICIAL_ROOT/c" \
  "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" "$OFFICIAL_LIB" \
  -o "$OFFICIAL_BUILD/bench"

"$CC_BIN" -O3 -std=c11 -I"$ROOT/c" \
  "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" "$FORK_LIB" \
  -o "$FORK_BUILD/bench"

printf 'mode,size_bytes,round,iterations,seconds,ns_per_op,mib_s,guard,digest\n' \
  >"$OUT/results.csv"

iterations_for() {
  python3 - "$1" "$TARGET_MIB" <<'PY'
import math,sys
size=int(sys.argv[1])
target=int(sys.argv[2])*1024*1024
print(max(1, math.ceil(target/size)))
PY
}

append_result() {
  local bin="$1" mode="$2" size="$3" iters="$4" round="$5"
  local line
  line="$("$bin" "$mode" "$size" "$iters")"
  printf '%s\n' "$line" | awk -F, -v r="$round" '
    $1=="SIZE_RESULT" {
      printf "%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
        $2,$3,r,$4,$5,$6,$7,$8,$9
    }' >>"$OUT/results.csv"
}

for size in $SIZES; do
  iters="$(iterations_for "$size")"
  "$OFFICIAL_BUILD/bench" warmup "$size" "$iters" >/dev/null
  "$FORK_BUILD/bench" warmup "$size" "$iters" >/dev/null

  # Alternate execution order to reduce systematic first/second-run bias.
  for ((round=1; round<=ROUNDS; round++)); do
    if (( round % 2 == 1 )); then
      append_result "$OFFICIAL_BUILD/bench" official "$size" "$iters" "$round"
      append_result "$FORK_BUILD/bench" fork "$size" "$iters" "$round"
    else
      append_result "$FORK_BUILD/bench" fork "$size" "$iters" "$round"
      append_result "$OFFICIAL_BUILD/bench" official "$size" "$iters" "$round"
    fi
  done
done

python3 "$ROOT/rmr/benchmark_framework/simperf/analyze_blake3_compare_v2.py" \
  --csv "$OUT/results.csv" \
  --out "$OUT/summary.json" \
  | tee "$OUT/analysis.txt"

snapshot_runtime after

python3 "$ROOT/rmr/validation/analyze_physical_telemetry.py"   --before "$OUT/telemetry-before.txt"   --after "$OUT/telemetry-after.txt"   --out "$OUT/telemetry-delta.json"   | tee "$OUT/telemetry-analysis.txt"

{
  echo "schema=RMR-PHYSICAL-UPSTREAM-COMPARE-V3"
  echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "architecture=$ARCH"
  echo "profile=$PROFILE"
  echo "official_commit=$(git -C "$OFFICIAL_ROOT" rev-parse HEAD)"
  echo "fork_commit=$(git -C "$ROOT" rev-parse HEAD)"
  echo "compiler=$("$CC_BIN" --version | head -n1)"
  echo "cmake=$(cmake --version | head -n1)"
  echo "cflags=$CFLAGS"
  echo "rounds=$ROUNDS"
  echo "target_mib_per_measurement=$TARGET_MIB"
  echo "telemetry_before=telemetry-before.txt"
  echo "telemetry_after=telemetry-after.txt"
  echo "thermal_dvfs_interpretation=REQUIRED_FOR_PHYSICAL_PERFORMANCE_CLAIMS"
  echo "claim_allowed=false"
  if command -v getprop >/dev/null 2>&1; then
    echo "android_release=$(getprop ro.build.version.release 2>/dev/null || true)"
    echo "android_abi=$(getprop ro.product.cpu.abi 2>/dev/null || true)"
    echo "device_model=$(getprop ro.product.model 2>/dev/null || true)"
  fi
} >"$OUT/environment.txt"

# Capability information only. Deliberately avoid Serial/unique identifiers.
if [ -r /proc/cpuinfo ]; then
  grep -E '^(model name|Processor|Hardware|Features|CPU architecture|CPU implementer|CPU part)' \
    /proc/cpuinfo | head -n 100 >"$OUT/cpu-capabilities.txt" || true
fi

sha256sum "$OUT"/*.txt "$OUT"/*.csv "$OUT"/*.json "$OUT"/*.log \
  >"$OUT/SHA256SUMS.txt"

echo "RMR_PHYSICAL_UPSTREAM_COMPARE_V3=PASS"
echo "receipt_dir=$OUT"
