#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/upstream_validation/common.sh"
for t in git python3 cargo rustc; do rmr_need "$t"; done

WORK="${WORK_ROOT:-$ROOT/.rmr-work/upstream-v3-cross}"
OUT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/upstream-v3-cross}"
OFFICIAL_ROOT="$WORK/official"
rm -rf "$OUT" "$WORK/obj"
mkdir -p "$WORK/obj" "$OUT"
OFFICIAL_COMMIT="$(rmr_checkout_official "$OFFICIAL_ROOT")"
rmr_write_common_environment "$OUT/environment.txt" "$OFFICIAL_COMMIT"
printf 'side,target,profile,state\n' >"$OUT/matrix.csv"

compile_set() {
  local side="$1" src="$2" target="$3" profile="$4" cc="$5" flags="$6"
  local dir="$WORK/obj/$side-$target-$profile"
  mkdir -p "$dir"
  local defs="-DBLAKE3_USE_NEON=0 -DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512"
  # shellcheck disable=SC2086
  $cc $flags -std=c99 -O2 -Wall -Wextra -Werror -I"$src/c" $defs     -c "$src/c/blake3.c" -o "$dir/blake3.o"
  # shellcheck disable=SC2086
  $cc $flags -std=c99 -O2 -Wall -Wextra -Werror -I"$src/c" $defs     -c "$src/c/blake3_dispatch.c" -o "$dir/dispatch.o"
  # shellcheck disable=SC2086
  $cc $flags -std=c99 -O2 -Wall -Wextra -Werror -I"$src/c" $defs     -c "$src/c/blake3_portable.c" -o "$dir/portable.o"
  printf '%s,%s,%s,PASS\n' "$side" "$target" "$profile" >>"$OUT/matrix.csv"
}

compile_neon() {
  local side="$1" src="$2" target="$3" cc="$4" flags="$5"
  local dir="$WORK/obj/$side-$target-neon"
  mkdir -p "$dir"
  # shellcheck disable=SC2086
  $cc $flags -std=c99 -O2 -Wall -Wextra -Werror -I"$src/c"     -DBLAKE3_USE_NEON=1 -c "$src/c/blake3_neon.c" -o "$dir/neon.o"
  printf '%s,%s,neon,PASS\n' "$side" "$target" >>"$OUT/matrix.csv"
}

compile_x86_intrinsics() {
  local side="$1" src="$2"
  local cc="gcc -m32" dir="$WORK/obj/$side-x86_32-simd"
  mkdir -p "$dir"
  $cc -std=c99 -O2 -Wall -Wextra -Werror -I"$src/c" -msse2     -c "$src/c/blake3_sse2.c" -o "$dir/sse2.o"
  $cc -std=c99 -O2 -Wall -Wextra -Werror -I"$src/c" -msse4.1     -c "$src/c/blake3_sse41.c" -o "$dir/sse41.o"
  $cc -std=c99 -O2 -Wall -Wextra -Werror -I"$src/c" -mavx2     -c "$src/c/blake3_avx2.c" -o "$dir/avx2.o"
  $cc -std=c99 -O2 -Wall -Wextra -Werror -I"$src/c" -mavx512f -mavx512vl     -c "$src/c/blake3_avx512.c" -o "$dir/avx512.o"
  printf '%s,x86_32,simd-intrinsics,PASS\n' "$side" >>"$OUT/matrix.csv"
}

for spec in   "x86_64|gcc|"   "x86_32|gcc -m32|"   "armv7|arm-linux-gnueabihf-gcc|-march=armv7-a -mfloat-abi=hard"   "aarch64|aarch64-linux-gnu-gcc|"   "riscv64|riscv64-linux-gnu-gcc|"   "ppc64le|powerpc64le-linux-gnu-gcc|"; do
  IFS='|' read -r target cc flags <<<"$spec"
  command -v "${cc%% *}" >/dev/null
  compile_set official "$OFFICIAL_ROOT" "$target" portable "$cc" "$flags"
  compile_set fork "$RMR_REPO_ROOT" "$target" portable "$cc" "$flags"
done

compile_x86_intrinsics official "$OFFICIAL_ROOT"
compile_x86_intrinsics fork "$RMR_REPO_ROOT"
compile_neon official "$OFFICIAL_ROOT" armv7 arm-linux-gnueabihf-gcc "-march=armv7-a -mfpu=neon -mfloat-abi=hard"
compile_neon fork "$RMR_REPO_ROOT" armv7 arm-linux-gnueabihf-gcc "-march=armv7-a -mfpu=neon -mfloat-abi=hard"
compile_neon official "$OFFICIAL_ROOT" aarch64 aarch64-linux-gnu-gcc ""
compile_neon fork "$RMR_REPO_ROOT" aarch64 aarch64-linux-gnu-gcc ""

if [ -f "$RMR_REPO_ROOT/c/blake3_neon_armv7_unix.S" ]; then
  arm-linux-gnueabihf-gcc -march=armv7-a -c "$RMR_REPO_ROOT/c/blake3_neon_armv7_unix.S"     -o "$WORK/obj/fork-armv7-marker.o"
  echo "fork,armv7,extra-asm-marker,PASS" >>"$OUT/matrix.csv"
fi
if [ -f "$RMR_REPO_ROOT/c/blake3_neon_aarch64_unix.S" ]; then
  aarch64-linux-gnu-gcc -c "$RMR_REPO_ROOT/c/blake3_neon_aarch64_unix.S"     -o "$WORK/obj/fork-aarch64-marker.o"
  echo "fork,aarch64,extra-asm-marker,PASS" >>"$OUT/matrix.csv"
fi

cargo check --manifest-path "$OFFICIAL_ROOT/Cargo.toml" -p blake3   --target wasm32-unknown-unknown --no-default-features   >"$OUT/official-wasm.log" 2>&1
cargo check --manifest-path "$RMR_REPO_ROOT/Cargo.toml" -p blake3   --target wasm32-unknown-unknown --no-default-features   >"$OUT/fork-wasm.log" 2>&1
echo "official,wasm32,rust-no-default-features,PASS" >>"$OUT/matrix.csv"
echo "fork,wasm32,rust-no-default-features,PASS" >>"$OUT/matrix.csv"

python3 - "$OUT/matrix.csv" "$OUT/summary.json" <<'PY'
import csv,json,sys
rows=list(csv.DictReader(open(sys.argv[1],encoding="utf-8")))
assert rows and all(r["state"]=="PASS" for r in rows)
targets=sorted(set(r["target"] for r in rows))
json.dump({"schema":"RMR-CROSS-ARCH-V3","claim_allowed":False,
           "rows":rows,"targets":targets,
           "physical_execution":"TOKEN_VAZIO_EXCEPT_NATIVE_CI"},
          open(sys.argv[2],"w"),indent=2)
print(f"RMR_CROSS_ARCH_V3=PASS rows={len(rows)} targets={len(targets)}")
PY

{
 echo "RMR_CROSS_ARCH_V3=PASS"
 echo "physical_armv7=TOKEN_VAZIO"
 echo "physical_aarch64=TOKEN_VAZIO"
 echo "claim_allowed=false"
} >"$OUT/receipt.txt"
sha256sum "$OUT"/*.txt "$OUT"/*.csv "$OUT"/*.json "$OUT"/*.log >"$OUT/SHA256SUMS.txt"
