#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/upstream_validation/common.sh"
for t in git cmake "${CC:-clang}" python3; do rmr_need "$t"; done

WORK="${WORK_ROOT:-$ROOT/.rmr-work/upstream-v3-ablation}"
OUT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/upstream-v3-ablation}"
OFFICIAL_ROOT="$WORK/official"
ROUNDS="${ROUNDS:-7}"
TARGET_MIB="${TARGET_MIB:-64}"
SIZES="${SIZES:-64 1024 65536 1048576}"

rm -rf "$OUT" "$WORK/fork-"* "$WORK/build-"*
mkdir -p "$WORK" "$OUT"
OFFICIAL_COMMIT="$(rmr_checkout_official "$OFFICIAL_ROOT")"
rmr_write_common_environment "$OUT/environment.txt" "$OFFICIAL_COMMIT"

cp -a "$RMR_REPO_ROOT/c" "$WORK/fork-base"
cp -a "$RMR_REPO_ROOT/c" "$WORK/fork-no-likely"
cp -a "$RMR_REPO_ROOT/c" "$WORK/fork-no-restrict"
cp -a "$RMR_REPO_ROOT/c" "$WORK/fork-no-hints"

python3 - "$WORK" <<'PY'
from pathlib import Path
import sys
w=Path(sys.argv[1])
for name in ["fork-no-likely","fork-no-hints"]:
    p=w/name/"blake3_dispatch.c"
    s=p.read_text()
    s=s.replace("#define BLAKE3_LIKELY(x) __builtin_expect(!!(x), 1)",
                "#define BLAKE3_LIKELY(x) (x)")
    p.write_text(s)
for name in ["fork-no-restrict","fork-no-hints"]:
    p=w/name/"blake3.h"
    s=p.read_text()
    s=s.replace("#define BLAKE3_RESTRICT __restrict__",
                "#define BLAKE3_RESTRICT")
    p.write_text(s)
PY

printf 'variant,size_bytes,round,iterations,mib_s,digest\n' >"$OUT/results.csv"

build_variant() {
  local name="$1" src="$2" build="$WORK/build-$1"
  cmake -S "$src" -B "$build" -G Ninja     -DCMAKE_BUILD_TYPE=Release     -DCMAKE_C_COMPILER="${CC:-clang}"     -DBUILD_SHARED_LIBS=OFF -DBLAKE3_USE_TBB=OFF     >"$OUT/$name-configure.log" 2>&1
  cmake --build "$build" --parallel 2 >"$OUT/$name-build.log" 2>&1
  local lib
  lib="$(find "$build" -type f -name 'libblake3.a' -print -quit)"
  [ -n "$lib" ]
  "${CC:-clang}" -O3 -std=c11 -I"$src"     "$ROOT/benchmark_framework/core/blake3_size_bench.c" "$lib"     -o "$build/bench"
  for size in $SIZES; do
    local iterations
    iterations="$(python3 - "$size" "$TARGET_MIB" <<'PY'
import math,sys
print(max(1,math.ceil(int(sys.argv[2])*1024*1024/int(sys.argv[1]))))
PY
)"
    "$build/bench" warmup "$size" "$iterations" >/dev/null
    for ((round=1; round<=ROUNDS; round++)); do
      local line
      line="$("$build/bench" "$name" "$size" "$iterations")"
      printf '%s\n' "$line" | awk -F, -v v="$name" -v r="$round"         '$1=="SIZE_RESULT"{printf "%s,%s,%s,%s,%s,%s\n",v,$3,r,$4,$7,$9}'         >>"$OUT/results.csv"
    done
  done
}

build_variant upstream "$OFFICIAL_ROOT/c"
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
assert all(len(v)==1 for v in dig.values())
out=[]
for (variant,size),xs in sorted(g.items()):
    base=statistics.median(g[("fork",size)])
    med=statistics.median(xs)
    out.append({
      "variant":variant,"size_bytes":size,"rounds":len(xs),
      "median_mib_s":med,"mean_mib_s":statistics.fmean(xs),
      "cv_percent":statistics.stdev(xs)/statistics.fmean(xs)*100 if len(xs)>1 else 0.0,
      "ratio_vs_fork":med/base,"delta_vs_fork_percent":(med/base-1)*100
    })
json.dump({"schema":"RMR-C-ABLATION-V3","digest_equivalence":"PASS",
           "claim_allowed":False,"summary":out},open(sys.argv[2],"w"),indent=2)
print("RMR_C_ABLATION_V3=PASS")
for r in out: print(r)
PY

{
 echo "RMR_C_ABLATION_V3=PASS"
 echo "digest_equivalence=PASS"
 echo "claim_allowed=false"
} >"$OUT/receipt.txt"
sha256sum "$OUT"/*.txt "$OUT"/*.csv "$OUT"/*.json >"$OUT/SHA256SUMS.txt"
