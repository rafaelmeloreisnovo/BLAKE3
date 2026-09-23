#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/upstream_validation/common.sh"
for t in git cmake "${CC:-clang}" python3 awk; do rmr_need "$t"; done

WORK="${WORK_ROOT:-$ROOT/.rmr-work/upstream-v3-backends}"
OUT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/upstream-v3-backends}"
OFFICIAL_ROOT="$WORK/official"
ROUNDS="${ROUNDS:-5}"
TARGET_MIB="${TARGET_MIB:-64}"
SIZES="${SIZES:-1024 65536 1048576}"
rm -rf "$OUT" "$WORK/build-"*
mkdir -p "$WORK" "$OUT"
OFFICIAL_COMMIT="$(rmr_checkout_official "$OFFICIAL_ROOT")"
rmr_write_common_environment "$OUT/environment.txt" "$OFFICIAL_COMMIT"
mapfile -t GEN < <(rmr_generator_args)

printf 'side,variant,size_bytes,round,simd_degree,mib_s,digest\n' >"$OUT/results.csv"

iterations_for() {
  python3 - "$1" "$TARGET_MIB" <<'PY'
import math,sys
size=int(sys.argv[1]); mib=int(sys.argv[2])
print(max(1,math.ceil(mib*1024*1024/size)))
PY
}

variant_args() {
  case "$1" in
    portable) printf '%s\n' "-DBLAKE3_SIMD_TYPE=none" ;;
    sse2) printf '%s\n' "-DBLAKE3_SIMD_TYPE=amd64-asm" "-DCMAKE_C_FLAGS=-DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512" ;;
    sse41) printf '%s\n' "-DBLAKE3_SIMD_TYPE=amd64-asm" "-DCMAKE_C_FLAGS=-DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512" ;;
    avx2) printf '%s\n' "-DBLAKE3_SIMD_TYPE=amd64-asm" "-DCMAKE_C_FLAGS=-DBLAKE3_NO_AVX512" ;;
    auto_asm) printf '%s\n' "-DBLAKE3_SIMD_TYPE=amd64-asm" ;;
    auto_intrinsics) printf '%s\n' "-DBLAKE3_SIMD_TYPE=x86-intrinsics" ;;
    *) return 2 ;;
  esac
}

build_variant() {
  local side="$1" src="$2" variant="$3"
  local build="$WORK/build-$side-$variant"
  local args=()
  mapfile -t args < <(variant_args "$variant")
  rm -rf "$build"
  cmake -S "$src/c" -B "$build" "${GEN[@]}"     -DCMAKE_BUILD_TYPE=Release     -DCMAKE_C_COMPILER="${CC:-clang}"     -DCMAKE_CXX_COMPILER="${CXX:-clang++}"     -DBLAKE3_USE_TBB=OFF     -DBLAKE3_EXAMPLES=ON     "${args[@]}" >"$OUT/$side-$variant-configure.log" 2>&1
  cmake --build "$build" --parallel 2 >"$OUT/$side-$variant-build.log" 2>&1
  local lib
  lib="$(rmr_find_static_lib "$build")"
  [ -n "$lib" ]

  "${CC:-clang}" -O3 -std=c11 -I"$src/c"     "$ROOT/upstream_validation/blake3_backend_probe.c" "$lib"     -o "$build/backend-probe"
  "$build/backend-probe" >"$OUT/$side-$variant-probe.txt"
  local degree abc
  degree="$(awk -F= '$1=="simd_degree"{print $2}' "$OUT/$side-$variant-probe.txt")"
  abc="$(awk -F= '$1=="abc"{print $2}' "$OUT/$side-$variant-probe.txt")"
  [ "$abc" = "6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85" ]

  "${CC:-clang}" -O3 -std=c11 -I"$src/c"     "$ROOT/benchmark_framework/core/blake3_size_bench.c" "$lib"     -o "$build/size-bench"

  for size in $SIZES; do
    local iters
    iters="$(iterations_for "$size")"
    "$build/size-bench" warmup "$size" "$iters" >/dev/null
    for ((round=1; round<=ROUNDS; round++)); do
      local line mib digest
      line="$("$build/size-bench" "$side-$variant" "$size" "$iters")"
      mib="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $7}')"
      digest="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $9}')"
      printf '%s,%s,%s,%s,%s,%s,%s\n'         "$side" "$variant" "$size" "$round" "$degree" "$mib" "$digest"         >>"$OUT/results.csv"
    done
  done
}

for variant in portable sse2 sse41 avx2 auto_asm auto_intrinsics; do
  build_variant official "$OFFICIAL_ROOT" "$variant"
  build_variant fork "$RMR_REPO_ROOT" "$variant"
done

python3 "$ROOT/upstream_validation/analyze_backend_matrix.py"   "$OUT/results.csv" "$OUT/summary.json" | tee "$OUT/analysis.txt"

{
  echo "RMR_BACKEND_MATRIX_V3=PASS"
  echo "variants=6"
  echo "rounds=$ROUNDS"
  echo "sizes=$SIZES"
  echo "claim_allowed=false"
} >"$OUT/receipt.txt"
sha256sum "$OUT"/*.txt "$OUT"/*.csv "$OUT"/*.json >"$OUT/SHA256SUMS.txt"
