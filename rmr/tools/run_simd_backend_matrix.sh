#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
CC_BIN="${CC:-clang}"
WORK="${WORK:-$ROOT/.rmr-work/simd-matrix}"
OUT="${OUT:-$ROOT/rmr/validation/output/simd-matrix}"
ROUNDS="${ROUNDS:-5}"
TARGET_MIB="${TARGET_MIB:-64}"
SIZES="${SIZES:-64 1048576}"

rm -rf "$WORK" "$OUT"
mkdir -p "$WORK" "$OUT"
printf 'profile,size_bytes,round,iterations,mib_s,digest,simd_degree\n' >"$OUT/results.csv"

build_profile() {
  local name="$1"
  local defs="$2"
  local build="$WORK/$name"

  cmake -S "$ROOT/c" -B "$build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER="$CC_BIN" \
    -DBUILD_SHARED_LIBS=OFF \
    -DBLAKE3_USE_TBB=OFF \
    -DCMAKE_C_FLAGS="$defs" \
    >"$OUT/$name-configure.log" 2>&1
  cmake --build "$build" --parallel 2 >"$OUT/$name-build.log" 2>&1

  local lib
  lib="$(find "$build" -type f -name 'libblake3.a' -print -quit)"
  [ -n "$lib" ] || { echo "missing lib for $name" >&2; exit 1; }

  "$CC_BIN" -O3 -std=c11 -I"$ROOT/c" \
    "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" "$lib" \
    -o "$build/size-bench"

  "$CC_BIN" -O2 -std=c11 -I"$ROOT/c" \
    "$ROOT/rmr/validation/blake3_backend_probe.c" "$lib" \
    -o "$build/backend-probe"

  local degree
  degree="$("$build/backend-probe" | awk -F= '/simd_degree/{print $2}')"

  for size in $SIZES; do
    local iterations
    iterations="$(python3 - "$size" "$TARGET_MIB" <<'PY'
import math,sys
size=int(sys.argv[1]); target=int(sys.argv[2])*1024*1024
print(max(1, math.ceil(target/size)))
PY
)"
    "$build/size-bench" warmup "$size" "$iterations" >/dev/null
    for ((round=1; round<=ROUNDS; round++)); do
      local line
      line="$("$build/size-bench" "$name" "$size" "$iterations")"
      printf '%s\n' "$line" | awk -F, -v profile="$name" -v round="$round" -v degree="$degree" '
        $1=="SIZE_RESULT" {
          printf "%s,%s,%s,%s,%s,%s,%s\n", profile,$3,round,$4,$7,$9,degree
        }' >>"$OUT/results.csv"
    done
  done
}

build_profile portable "-DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512"
build_profile sse2 "-DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512"
build_profile sse41 "-DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512"
build_profile avx2 "-DBLAKE3_NO_AVX512"
build_profile native ""

python3 - "$OUT/results.csv" "$OUT/summary.json" <<'PY'
import csv,json,statistics,sys
from collections import defaultdict
inp,out=sys.argv[1:]
rows=list(csv.DictReader(open(inp,encoding="utf-8")))
g=defaultdict(list); dig=defaultdict(set); degree={}
for r in rows:
    key=(r["profile"],int(r["size_bytes"]))
    g[key].append(float(r["mib_s"]))
    dig[int(r["size_bytes"])].add(r["digest"])
    degree[r["profile"]]=int(r["simd_degree"])
summary=[]
for key,xs in sorted(g.items()):
    profile,size=key
    summary.append({"profile":profile,"size_bytes":size,"rounds":len(xs),"median_mib_s":statistics.median(xs),"mean_mib_s":statistics.fmean(xs),"cv_percent":statistics.stdev(xs)/statistics.fmean(xs)*100 if len(xs)>1 else 0,"simd_degree":degree[profile]})
assert all(len(v)==1 for v in dig.values())
json.dump({"schema":"RMR-SIMD-BACKEND-MATRIX-V1","digest_equivalence":"PASS","summary":summary,"claim_allowed":False},open(out,"w"),indent=2)
print("RMR_SIMD_BACKEND_MATRIX=PASS")
for r in summary: print(r)
PY
