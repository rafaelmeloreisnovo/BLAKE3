#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-6aab490a26124663329dfd3961b8469f8fdb158b}"
CC_BIN="${CC:-clang}"
WORK="${WORK:-$ROOT/.rmr-work/c-ablation}"
OUT="${OUT:-$ROOT/rmr/validation/output/c-ablation}"
ROUNDS="${ROUNDS:-7}"
TARGET_MIB="${TARGET_MIB:-64}"
SIZES="${SIZES:-64 1024 65536 1048576}"

rm -rf "$WORK" "$OUT"
mkdir -p "$WORK" "$OUT"
git clone --filter=blob:none --no-tags "$OFFICIAL_REPO" "$WORK/upstream" >/dev/null 2>&1
git -C "$WORK/upstream" fetch --depth 1 origin "$OFFICIAL_REF" >/dev/null 2>&1
git -C "$WORK/upstream" checkout --detach FETCH_HEAD >/dev/null 2>&1

cp -a "$ROOT/c" "$WORK/fork-base"
cp -a "$ROOT/c" "$WORK/fork-no-likely"
cp -a "$ROOT/c" "$WORK/fork-no-restrict"
cp -a "$ROOT/c" "$WORK/fork-no-hints"

python3 - "$WORK" <<'PY'
from pathlib import Path
import sys
w=Path(sys.argv[1])
for name in ["fork-no-likely","fork-no-hints"]:
    p=w/name/"blake3_dispatch.c"
    s=p.read_text()
    s=s.replace("#define BLAKE3_LIKELY(x) __builtin_expect(!!(x), 1)", "#define BLAKE3_LIKELY(x) (x)")
    p.write_text(s)
for name in ["fork-no-restrict","fork-no-hints"]:
    p=w/name/"blake3.h"
    s=p.read_text()
    s=s.replace("#define BLAKE3_RESTRICT __restrict__", "#define BLAKE3_RESTRICT")
    p.write_text(s)
PY

printf 'variant,size_bytes,round,iterations,mib_s,digest\n' >"$OUT/results.csv"

build_variant() {
  local name="$1"; local src="$2"; local build="$WORK/build-$name"
  cmake -S "$src" -B "$build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="$CC_BIN" \
    -DBUILD_SHARED_LIBS=OFF -DBLAKE3_USE_TBB=OFF \
    >"$OUT/$name-configure.log" 2>&1
  cmake --build "$build" --parallel 2 >"$OUT/$name-build.log" 2>&1
  local lib="$(find "$build" -type f -name 'libblake3.a' -print -quit)"
  [ -n "$lib" ] || { echo "missing lib $name" >&2; exit 1; }
  "$CC_BIN" -O3 -std=c11 -I"$src" "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" "$lib" -o "$build/bench"
  for size in $SIZES; do
    local iterations="$(python3 - "$size" "$TARGET_MIB" <<'PY'
import math,sys
print(max(1,math.ceil(int(sys.argv[2])*1024*1024/int(sys.argv[1]))))
PY
)"
    "$build/bench" warmup "$size" "$iterations" >/dev/null
    for ((round=1; round<=ROUNDS; round++)); do
      local line="$("$build/bench" "$name" "$size" "$iterations")"
      printf '%s\n' "$line" | awk -F, -v v="$name" -v r="$round" '$1=="SIZE_RESULT"{printf "%s,%s,%s,%s,%s,%s\n",v,$3,r,$4,$7,$9}' >>"$OUT/results.csv"
    done
  done
}

build_variant upstream "$WORK/upstream/c"
build_variant fork "$WORK/fork-base"
build_variant fork_no_likely "$WORK/fork-no-likely"
build_variant fork_no_restrict "$WORK/fork-no-restrict"
build_variant fork_no_hints "$WORK/fork-no-hints"

python3 - "$OUT/results.csv" "$OUT/summary.json" <<'PY'
import csv,json,statistics,sys
from collections import defaultdict
rows=list(csv.DictReader(open(sys.argv[1],encoding="utf-8")))
g=defaultdict(list); dig=defaultdict(set)
for r in rows:
    key=(r["variant"],int(r["size_bytes"]))
    g[key].append(float(r["mib_s"]))
    dig[int(r["size_bytes"])].add(r["digest"])
assert all(len(x)==1 for x in dig.values())
out=[]
for (variant,size),xs in sorted(g.items()):
    base=statistics.median(g[("fork",size)])
    med=statistics.median(xs)
    out.append({"variant":variant,"size_bytes":size,"rounds":len(xs),"median_mib_s":med,"mean_mib_s":statistics.fmean(xs),"cv_percent":statistics.stdev(xs)/statistics.fmean(xs)*100 if len(xs)>1 else 0.0,"ratio_vs_fork":med/base,"delta_vs_fork_percent":(med/base-1)*100})
json.dump({"schema":"RMR-C-ABLATION-V1","digest_equivalence":"PASS","claim_allowed":False,"summary":out},open(sys.argv[2],"w"),indent=2)
print("RMR_C_ABLATION=PASS")
for r in out: print(r)
PY
