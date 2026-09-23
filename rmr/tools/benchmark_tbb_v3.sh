#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/upstream_validation/common.sh"
for t in git cmake "${CC:-clang}" "${CXX:-clang++}" python3; do rmr_need "$t"; done

WORK="${WORK_ROOT:-$ROOT/.rmr-work/upstream-v3-tbb}"
OUT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/upstream-v3-tbb}"
OFFICIAL_ROOT="$WORK/official"
ROUNDS="${ROUNDS:-5}"
rm -rf "$OUT" "$WORK/build-"*
mkdir -p "$WORK" "$OUT"
OFFICIAL_COMMIT="$(rmr_checkout_official "$OFFICIAL_ROOT")"
rmr_write_common_environment "$OUT/environment.txt" "$OFFICIAL_COMMIT"
mapfile -t GEN < <(rmr_generator_args)

printf 'side,size_bytes,round,mib_s,digest\n' >"$OUT/results.csv"

build_side() {
  local side="$1" src="$2" build="$WORK/build-$1"
  cmake -S "$src/c" -B "$build" "${GEN[@]}"     -DCMAKE_BUILD_TYPE=Release     -DCMAKE_C_COMPILER="${CC:-clang}"     -DCMAKE_CXX_COMPILER="${CXX:-clang++}"     -DBLAKE3_USE_TBB=ON     -DBLAKE3_FETCH_TBB=OFF     >"$OUT/$side-configure.log" 2>&1
  cmake --build "$build" --parallel 2 >"$OUT/$side-build.log" 2>&1
  local lib
  lib="$(rmr_find_static_lib "$build")"; [ -n "$lib" ]

  "${CC:-clang}" -O3 -std=c11 -DBLAKE3_USE_TBB -I"$src/c"     -c "$ROOT/upstream_validation/blake3_tbb_bench.c"     -o "$build/tbb-bench.o"
  "${CXX:-clang++}" "$build/tbb-bench.o" "$lib" -ltbb -pthread     -o "$build/tbb-bench"

  for size in 1048576 16777216; do
    local iters
    if [ "$size" -eq 1048576 ]; then iters=128; else iters=8; fi
    "$build/tbb-bench" "$size" 1 >/dev/null
    for ((r=1;r<=ROUNDS;r++)); do
      local line mib digest
      line="$("$build/tbb-bench" "$size" "$iters")"
      mib="$(printf '%s\n' "$line" | awk -F, '$1=="TBB_RESULT"{for(i=1;i<=NF;i++) if($i ~ /^mib_s=/){split($i,a,"=");print a[2]}}')"
      digest="$(printf '%s\n' "$line" | awk -F'digest=' '{print $2}')"
      printf '%s,%s,%s,%s,%s\n' "$side" "$size" "$r" "$mib" "$digest" >>"$OUT/results.csv"
    done
  done
}

build_side official "$OFFICIAL_ROOT"
build_side fork "$RMR_REPO_ROOT"

python3 - "$OUT/results.csv" "$OUT/summary.json" <<'PY'
import csv,json,statistics,sys
from collections import defaultdict
rows=list(csv.DictReader(open(sys.argv[1],encoding="utf-8")))
g=defaultdict(lambda:defaultdict(list)); d=defaultdict(set)
for r in rows:
    k=int(r["size_bytes"]); s=r["side"]
    g[k][s].append(float(r["mib_s"])); d[(k,s)].add(r["digest"])
out=[]
for k in sorted(g):
    assert d[(k,"official")]==d[(k,"fork")]
    o=statistics.median(g[k]["official"]); f=statistics.median(g[k]["fork"])
    out.append({"size_bytes":k,"official_median_mib_s":o,"fork_median_mib_s":f,
                "delta_percent":(f/o-1)*100,"digest_equivalence":"PASS"})
json.dump({"schema":"RMR-TBB-V3","claim_allowed":False,"rows":out},open(sys.argv[2],"w"),indent=2)
print("RMR_TBB_V3=PASS")
for x in out: print(x)
PY

{
 echo "RMR_TBB_V3=PASS"
 echo "digest_equivalence=PASS"
 echo "claim_allowed=false"
} >"$OUT/receipt.txt"
sha256sum "$OUT"/*.txt "$OUT"/*.csv "$OUT"/*.json >"$OUT/SHA256SUMS.txt"
