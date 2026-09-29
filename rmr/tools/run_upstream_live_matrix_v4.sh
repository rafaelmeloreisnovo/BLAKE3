#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# Live official-upstream vs fork compile/performance matrix.
# SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF_REQUESTED="${OFFICIAL_REF_REQUESTED:-refs/heads/master}"
MATRIX_MODE="${MATRIX_MODE:-balanced}"
ROUNDS="${ROUNDS:-7}"
TARGET_MIB="${TARGET_MIB:-64}"
SIZES="${SIZES:-64 1024 65536 1048576}"
JOBS="${JOBS:-2}"
OUT="$ROOT/rmr/benchmark_framework/output/upstream-live-v4"
WORK="$ROOT/.rmr-work/upstream-live-v4"
OFFICIAL_ROOT="$WORK/official"
EXPECTED_ABC="6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85"

case "$MATRIX_MODE" in balanced|exhaustive) ;; *) echo "matrix_mode=FAIL:$MATRIX_MODE" >&2; exit 2;; esac
case "$ROUNDS" in ''|*[!0-9]*) echo "rounds=FAIL:$ROUNDS" >&2; exit 2;; esac
case "$TARGET_MIB" in ''|*[!0-9]*) echo "target_mib=FAIL:$TARGET_MIB" >&2; exit 2;; esac
[ "$ROUNDS" -ge 3 ] || { echo "rounds_must_be_at_least_3" >&2; exit 2; }
[ "$TARGET_MIB" -ge 1 ] || { echo "target_mib_must_be_positive" >&2; exit 2; }
[ "$OFFICIAL_REPO" = "https://github.com/BLAKE3-team/BLAKE3.git" ] || { echo "official_repo_authority=FAIL:$OFFICIAL_REPO" >&2; exit 3; }
[ "$OFFICIAL_REF_REQUESTED" = "refs/heads/master" ] || { echo "official_ref_authority=FAIL:$OFFICIAL_REF_REQUESTED" >&2; exit 3; }

need() { command -v "$1" >/dev/null 2>&1 || { echo "missing_command=$1" >&2; exit 127; }; }
for t in git cmake ninja clang gcc g++ python3 awk sha256sum stat uname nproc; do need "$t"; done
rm -rf "$OUT" "$WORK"
mkdir -p "$OUT" "$WORK"

OFFICIAL_SHA="$(git ls-remote "$OFFICIAL_REPO" "$OFFICIAL_REF_REQUESTED" | awk 'NR==1{print $1}')"
printf '%s' "$OFFICIAL_SHA" | grep -Eq '^[0-9a-f]{40}$' || { echo "official_sha_resolution=FAIL:$OFFICIAL_SHA" >&2; exit 4; }
git init -q "$OFFICIAL_ROOT"
git -C "$OFFICIAL_ROOT" remote add origin "$OFFICIAL_REPO"
git -C "$OFFICIAL_ROOT" fetch -q --no-tags --depth=1 origin "$OFFICIAL_SHA"
git -C "$OFFICIAL_ROOT" checkout -q --detach FETCH_HEAD
[ "$(git -C "$OFFICIAL_ROOT" rev-parse HEAD)" = "$OFFICIAL_SHA" ] || exit 5
[ "$(git -C "$OFFICIAL_ROOT" remote get-url origin)" = "$OFFICIAL_REPO" ] || exit 6

FORK_SHA="$(git -C "$ROOT" rev-parse HEAD)"
FORK_TREE="$(git -C "$ROOT" rev-parse HEAD:c)"
OFFICIAL_TREE="$(git -C "$OFFICIAL_ROOT" rev-parse HEAD:c)"
(
  cd "$OFFICIAL_ROOT"
  find c -type f -print0 | sort -z | xargs -0 sha256sum
) > "$OUT/official-c-files.sha256"
(
  cd "$ROOT"
  find c -type f -print0 | sort -z | xargs -0 sha256sum
) > "$OUT/fork-c-files.sha256"
OFFICIAL_MANIFEST_SHA="$(sha256sum "$OUT/official-c-files.sha256" | awk '{print $1}')"
FORK_MANIFEST_SHA="$(sha256sum "$OUT/fork-c-files.sha256" | awk '{print $1}')"
diff -u "$OUT/official-c-files.sha256" "$OUT/fork-c-files.sha256" > "$OUT/source-manifest.diff" || true

{
  echo "schema=RMR-VM-FINGERPRINT-V1"
  echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "runner_os=${RUNNER_OS:-TOKEN_VAZIO}"
  echo "runner_arch=${RUNNER_ARCH:-TOKEN_VAZIO}"
  echo "image_os=${ImageOS:-TOKEN_VAZIO}"
  echo "image_version=${ImageVersion:-TOKEN_VAZIO}"
  echo "uname=$(uname -a)"
  echo "nproc=$(nproc)"
  echo "cpu_model=$(awk -F: '/model name/{gsub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo 2>/dev/null || true)"
  echo "cpu_flags=$(awk -F: '/^flags/{gsub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo 2>/dev/null || true)"
  echo "mem_total=$(awk '/MemTotal/{print $2" "$3}' /proc/meminfo 2>/dev/null || true)"
  echo "clocksource=$(cat /sys/devices/system/clocksource/clocksource0/current_clocksource 2>/dev/null || echo TOKEN_VAZIO)"
  echo "cgroup=$(tr '\n' ';' </proc/self/cgroup 2>/dev/null || echo TOKEN_VAZIO)"
  echo "clang=$(clang --version | head -n1)"
  echo "gcc=$(gcc --version | head -n1)"
  echo "cmake=$(cmake --version | head -n1)"
} > "$OUT/vm-fingerprint.txt"

printf 'config_id,compiler,simd,cap,opt,lto,tbb,size_bytes,round,side,iterations,seconds,ns_per_op,mib_s,digest,lib_size_bytes,lib_sha256,config_sha256\n' > "$OUT/results.csv"
printf 'config_id,compiler,simd,cap,opt,lto,tbb,config_sha256\n' > "$OUT/configs.csv"

iterations_for() {
  python3 - "$1" "$TARGET_MIB" <<'PY'
import math,sys
size=int(sys.argv[1]); mib=int(sys.argv[2])
print(max(1, math.ceil(mib*1024*1024/size)))
PY
}
cap_flags() {
  case "$1" in
    portable) printf '%s\n' "-DBLAKE3_SIMD_TYPE=none" ;;
    sse2) printf '%s\n' "-DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512" ;;
    sse41) printf '%s\n' "-DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512" ;;
    avx2) printf '%s\n' "-DBLAKE3_NO_AVX512" ;;
    auto) printf '%s\n' "" ;;
    *) return 2 ;;
  esac
}
compiler_cxx() { case "$1" in clang) echo clang++;; gcc) echo g++;; *) return 2;; esac; }

build_side() {
  local side="$1" src="$2" config="$3" compiler="$4" simd="$5" cap="$6" opt="$7" lto="$8" tbb="$9"
  local build="$WORK/build-$side-$config" cxx cap_cflags cflags ipo=OFF tbb_bool=OFF
  cxx="$(compiler_cxx "$compiler")"
  cap_cflags="$(cap_flags "$cap")"
  cflags="-$opt"; [ -z "$cap_cflags" ] || cflags="$cflags $cap_cflags"
  [ "$lto" = "on" ] && ipo=ON
  [ "$tbb" = "on" ] && tbb_bool=ON
  rm -rf "$build"
  cmake -S "$src/c" -B "$build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="$compiler" -DCMAKE_CXX_COMPILER="$cxx" -DCMAKE_C_FLAGS="$cflags" -DCMAKE_INTERPROCEDURAL_OPTIMIZATION="$ipo" -DBUILD_SHARED_LIBS=OFF -DBLAKE3_USE_TBB="$tbb_bool" -DBLAKE3_FETCH_TBB=OFF -DBLAKE3_EXAMPLES=OFF -DBLAKE3_SIMD_TYPE="$simd" >"$OUT/$config-$side-configure.log" 2>&1
  cmake --build "$build" --parallel "$JOBS" >"$OUT/$config-$side-build.log" 2>&1
  local lib; lib="$(find "$build" -type f -name 'libblake3.a' -print -quit)"
  [ -n "$lib" ] || { echo "static_library=TOKEN_VAZIO side=$side config=$config" >&2; exit 20; }
  local link_lto=() extra_link=()
  if [ "$lto" = "on" ]; then
    if [ "$compiler" = "clang" ]; then link_lto=(-flto -fuse-ld=lld); else link_lto=(-flto); fi
  fi
  [ "$tbb" = "on" ] && extra_link=(-ltbb -lstdc++)
  "$compiler" -O3 -std=c11 -I"$src/c" "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" "$lib" "${link_lto[@]}" "${extra_link[@]}" -o "$build/bench"
  echo "$build/bench|$lib"
}

append_result() {
  local bench="$1" lib="$2" side="$3" config="$4" compiler="$5" simd="$6" cap="$7" opt="$8" lto="$9" tbb="${10}" size="${11}" round="${12}" iterations="${13}" config_sha="${14}"
  local line sec ns mib digest lib_size lib_sha
  line="$("$bench" "$side" "$size" "$iterations")"
  sec="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $5}')"
  ns="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $6}')"
  mib="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $7}')"
  digest="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $9}')"
  lib_size="$(stat -c%s "$lib")"; lib_sha="$(sha256sum "$lib" | awk '{print $1}')"
  printf '%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n' "$config" "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb" "$size" "$round" "$side" "$iterations" "$sec" "$ns" "$mib" "$digest" "$lib_size" "$lib_sha" "$config_sha" >> "$OUT/results.csv"
}

run_config() {
  local compiler="$1" simd="$2" cap="$3" opt="$4" lto="$5" tbb="$6"
  local config="${compiler}_${simd}_${cap}_${opt}_lto-${lto}_tbb-${tbb}" canonical config_sha
  config="${config//[^A-Za-z0-9_.-]/-}"
  canonical="compiler=$compiler;simd=$simd;cap=$cap;opt=$opt;lto=$lto;tbb=$tbb"
  config_sha="$(printf '%s' "$canonical" | sha256sum | awk '{print $1}')"
  printf '%s,%s,%s,%s,%s,%s,%s,%s\n' "$config" "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb" "$config_sha" >> "$OUT/configs.csv"
  local o_pair f_pair o_bench o_lib f_bench f_lib
  o_pair="$(build_side official "$OFFICIAL_ROOT" "$config" "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb")"
  f_pair="$(build_side fork "$ROOT" "$config" "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb")"
  o_bench="${o_pair%%|*}"; o_lib="${o_pair#*|}"; f_bench="${f_pair%%|*}"; f_lib="${f_pair#*|}"
  for size in $SIZES; do
    local iters od fd; iters="$(iterations_for "$size")"
    "$o_bench" warmup "$size" "$iters" >/dev/null; "$f_bench" warmup "$size" "$iters" >/dev/null
    od="$("$o_bench" official "$size" 1 | awk -F, '$1=="SIZE_RESULT"{print $9}')"; fd="$("$f_bench" fork "$size" 1 | awk -F, '$1=="SIZE_RESULT"{print $9}')"
    [ "$od" = "$fd" ] || { echo "digest_equivalence=FAIL config=$config size=$size" >&2; exit 30; }
    for ((round=1; round<=ROUNDS; round++)); do
      if (( round % 2 == 1 )); then
        append_result "$o_bench" "$o_lib" official "$config" "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb" "$size" "$round" "$iters" "$config_sha"
        append_result "$f_bench" "$f_lib" fork "$config" "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb" "$size" "$round" "$iters" "$config_sha"
      else
        append_result "$f_bench" "$f_lib" fork "$config" "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb" "$size" "$round" "$iters" "$config_sha"
        append_result "$o_bench" "$o_lib" official "$config" "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb" "$size" "$round" "$iters" "$config_sha"
      fi
    done
  done
}

if [ "$MATRIX_MODE" = "balanced" ]; then
  for compiler in clang gcc; do
    run_config "$compiler" none portable O3 off off
    run_config "$compiler" amd64-asm sse2 O3 off off
    run_config "$compiler" amd64-asm sse41 O3 off off
    run_config "$compiler" amd64-asm avx2 O3 off off
    run_config "$compiler" amd64-asm auto O3 off off
    run_config "$compiler" x86-intrinsics auto O3 off off
  done
  run_config clang amd64-asm auto O3 on off
  run_config clang x86-intrinsics auto O3 on off
  run_config clang amd64-asm auto O2 off off
  run_config clang amd64-asm auto O3 off on
else
  for compiler in clang gcc; do
    for simd in amd64-asm x86-intrinsics; do
      for cap in sse2 sse41 avx2 auto; do
        for opt in O2 O3; do
          for lto in off on; do
            for tbb in off on; do run_config "$compiler" "$simd" "$cap" "$opt" "$lto" "$tbb"; done
          done
        done
      done
    done
    for opt in O2 O3; do
      for lto in off on; do
        for tbb in off on; do run_config "$compiler" none portable "$opt" "$lto" "$tbb"; done
      done
    done
  done
fi

python3 "$ROOT/rmr/upstream_validation/analyze_live_matrix_v4.py" --csv "$OUT/results.csv" --json "$OUT/summary.json" --md "$OUT/summary.md"
{
  echo "schema=RMR-UPSTREAM-LIVE-COMPILE-MATRIX-V4"
  echo "official_repository=$OFFICIAL_REPO"
  echo "official_ref_requested=$OFFICIAL_REF_REQUESTED"
  echo "official_sha_resolved=$OFFICIAL_SHA"
  echo "official_c_tree=$OFFICIAL_TREE"
  echo "official_c_manifest_sha256=$OFFICIAL_MANIFEST_SHA"
  echo "fork_sha=$FORK_SHA"
  echo "fork_c_tree=$FORK_TREE"
  echo "fork_c_manifest_sha256=$FORK_MANIFEST_SHA"
  echo "matrix_mode=$MATRIX_MODE"
  echo "rounds=$ROUNDS"
  echo "target_mib=$TARGET_MIB"
  echo "sizes=$SIZES"
  echo "source_authority=PASS_DIRECT_OFFICIAL_REMOTE"
  echo "paired_order=AB_BA"
  echo "digest_equivalence=PASS_ALL_EXECUTED_CONFIGS"
  echo "one_bit_visibility=SOURCE_FILE_SHA256+GIT_TREE+CONFIG_SHA256+BINARY_SHA256"
  echo "claim_allowed=false"
  echo "physical_device_reproduction=TOKEN_VAZIO"
  echo "independent_third_party_reproduction=TOKEN_VAZIO"
} > "$OUT/receipt.txt"
sha256sum "$OUT"/*.csv "$OUT"/*.json "$OUT"/*.md "$OUT"/*.txt "$OUT"/*.sha256 > "$OUT/SHA256SUMS.txt"
echo "RMR_UPSTREAM_LIVE_MATRIX_V4=PASS"
cat "$OUT/receipt.txt"
