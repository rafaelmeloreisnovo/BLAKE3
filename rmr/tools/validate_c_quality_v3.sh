#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/upstream_validation/common.sh"

for t in git cmake "${CC:-clang}" "${CXX:-clang++}" ctest pkg-config diff; do
  rmr_need "$t"
done

WORK="${WORK_ROOT:-$ROOT/.rmr-work/upstream-v3-quality}"
OUT="${RESULT_ROOT:-$ROOT/benchmark_framework/output/upstream-v3-quality}"
OFFICIAL_ROOT="$WORK/official"
rm -rf "$WORK/build-"* "$OUT"
mkdir -p "$WORK" "$OUT"

OFFICIAL_COMMIT="$(rmr_checkout_official "$OFFICIAL_ROOT")"
rmr_write_common_environment "$OUT/environment.txt" "$OFFICIAL_COMMIT"

mapfile -t GEN < <(rmr_generator_args)

build_side() {
  local side="$1"
  local src="$2"
  local build="$WORK/build-$side-strict"
  local prefix="$WORK/install-$side"

  rm -rf "$build" "$prefix"
  cmake -S "$src/c" -B "$build" "${GEN[@]}"     -DCMAKE_BUILD_TYPE=Release     -DCMAKE_C_COMPILER="${CC:-clang}"     -DCMAKE_CXX_COMPILER="${CXX:-clang++}"     -DCMAKE_COMPILE_WARNING_AS_ERROR=ON     -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic -Werror"     -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Werror"     -DBLAKE3_TESTING=ON     -DBLAKE3_EXAMPLES=ON     -DBLAKE3_USE_TBB=OFF     -DCMAKE_INSTALL_PREFIX="$prefix"     >"$OUT/$side-strict-configure.log" 2>&1

  cmake --build "$build" --parallel 2 >"$OUT/$side-strict-build.log" 2>&1
  ctest --test-dir "$build" --output-on-failure >"$OUT/$side-ctest.log" 2>&1
  cmake --install "$build" >"$OUT/$side-install.log" 2>&1

  local lib
  lib="$(rmr_find_static_lib "$build")"
  [ -n "$lib" ] || { echo "$side libblake3.a missing" >&2; return 1; }

  "${CC:-clang}" -O2 -std=c11 -I"$src/c"     "$ROOT/upstream_validation/blake3_modes_probe.c" "$lib"     -o "$WORK/$side-modes"
  "$WORK/$side-modes" >"$OUT/$side-modes.txt"

  "${CC:-clang}" -O2 -std=c11 -I"$src/c"     "$ROOT/upstream_validation/blake3_alignment_probe.c" "$lib"     -o "$WORK/$side-alignment"
  "$WORK/$side-alignment" >"$OUT/$side-alignment.txt"

  local pc
  pc="$(find "$prefix" -type f -name '*.pc' -print -quit)"
  [ -n "$pc" ] || { echo "$side pkg-config metadata missing" >&2; return 1; }
  local pcdir module
  pcdir="$(dirname "$pc")"
  module="$(basename "$pc" .pc)"

  cat >"$WORK/consumer.c" <<'C'
#include <blake3.h>
#include <stdio.h>
int main(void) {
  const unsigned char abc[3]={'a','b','c'};
  unsigned char out[BLAKE3_OUT_LEN];
  blake3_hasher h;
  size_t i;
  blake3_hasher_init(&h);
  blake3_hasher_update(&h,abc,sizeof(abc));
  blake3_hasher_finalize(&h,out,sizeof(out));
  for(i=0;i<sizeof(out);++i) printf("%02x",out[i]);
  putchar('\n');
  return 0;
}
C
  PKG_CONFIG_PATH="$pcdir" "${CC:-clang}" "$WORK/consumer.c"     $(PKG_CONFIG_PATH="$pcdir" pkg-config --cflags --libs "$module")     -o "$WORK/$side-consumer"
  "$WORK/$side-consumer" >"$OUT/$side-consumer.txt"

  grep -qx '6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85'     "$OUT/$side-consumer.txt"
}

sanitize_side() {
  local side="$1"
  local src="$2"
  local build="$WORK/build-$side-sanitize"
  rm -rf "$build"

  cmake -S "$src/c" -B "$build" "${GEN[@]}"     -DCMAKE_BUILD_TYPE=Debug     -DCMAKE_C_COMPILER="${CC:-clang}"     -DCMAKE_CXX_COMPILER="${CXX:-clang++}"     -DBLAKE3_TESTING=ON     -DBLAKE3_EXAMPLES=ON     -DBLAKE3_SIMD_TYPE=none     -DBLAKE3_USE_TBB=OFF     -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer"     -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"     >"$OUT/$side-sanitize-configure.log" 2>&1

  cmake --build "$build" --parallel 2 >"$OUT/$side-sanitize-build.log" 2>&1
  ASAN_OPTIONS=detect_leaks=1:halt_on_error=1   UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1     ctest --test-dir "$build" --output-on-failure     >"$OUT/$side-sanitize-ctest.log" 2>&1
}

build_side official "$OFFICIAL_ROOT"
build_side fork "$RMR_REPO_ROOT"

diff -u "$OUT/official-modes.txt" "$OUT/fork-modes.txt" >"$OUT/modes.diff"
diff -u "$OUT/official-alignment.txt" "$OUT/fork-alignment.txt" >"$OUT/alignment.diff"

sanitize_side official "$OFFICIAL_ROOT"
sanitize_side fork "$RMR_REPO_ROOT"

{
  echo "RMR_C_QUALITY_V3=PASS"
  echo "strict_warnings=PASS_BOTH"
  echo "ctest=PASS_BOTH"
  echo "semantic_modes_equivalence=PASS"
  echo "alignment_0_63_equivalence=PASS"
  echo "asan_ubsan=PASS_BOTH_PORTABLE"
  echo "cmake_install_pkgconfig_consumer=PASS_BOTH"
  echo "physical_cross_arch=TOKEN_VAZIO"
  echo "claim_allowed=false"
} | tee "$OUT/receipt.txt"

sha256sum "$OUT"/* >"$OUT/SHA256SUMS.txt"
