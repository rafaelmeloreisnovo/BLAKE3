#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
FORK_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-master}"
ROUNDS="${ROUNDS:-5}"
ITERATIONS_MIB="${ITERATIONS_MIB:-512}"
JOBS="${JOBS:-2}"
WORK_ROOT="${WORK_ROOT:-$FORK_ROOT/.bench/armv7-fork-vs-upstream}"
RESULT_ROOT="${RESULT_ROOT:-$FORK_ROOT/audit/results/armv7}"
STAMP="$(date -u +%Y%m%dT%H%M%SZ)"
RUN_DIR="$RESULT_ROOT/$STAMP"
OFFICIAL_ROOT="$WORK_ROOT/official"
FORK_BUILD="$WORK_ROOT/build-fork-neon"
OFFICIAL_BUILD="$WORK_ROOT/build-official-neon"
RESULTS="$RUN_DIR/results.csv"
SUMMARY="$RUN_DIR/summary.txt"
RECEIPT="$RUN_DIR/receipt.txt"
EXPECTED_ABC="6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85"
COMMON_FLAGS="-O3 -DNDEBUG -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=softfp"
NEON_FLAGS="-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=softfp"

require() {
  command -v "$1" >/dev/null 2>&1 || {
    printf 'missing_command=%s\n' "$1" >&2
    exit 127
  }
}

for tool in git cmake ninja clang awk sort sha256sum uname date tee; do
  require "$tool"
done

case "$(uname -m)" in
  armv7l|armv8l) ;;
  *) printf 'warning=benchmark designed for ARMv7; detected=%s\n' "$(uname -m)" ;;
esac

mkdir -p "$WORK_ROOT" "$RUN_DIR"

if [ ! -d "$OFFICIAL_ROOT/.git" ]; then
  git clone --filter=blob:none --no-tags "$OFFICIAL_REPO" "$OFFICIAL_ROOT"
fi

git -C "$OFFICIAL_ROOT" fetch --prune origin "$OFFICIAL_REF"
git -C "$OFFICIAL_ROOT" checkout --detach FETCH_HEAD

build_neon() {
  local source_root="$1"
  local build_root="$2"

  rm -rf "$build_root"
  cmake -S "$source_root/c" -B "$build_root" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_C_FLAGS_RELEASE="$COMMON_FLAGS" \
    -DBUILD_SHARED_LIBS=OFF \
    -DBLAKE3_USE_TBB=OFF \
    -DBLAKE3_EXAMPLES=ON \
    -DBLAKE3_SIMD_TYPE=neon-intrinsics \
    -DBLAKE3_CFLAGS_NEON="$NEON_FLAGS"
  cmake --build "$build_root" --parallel "$JOBS"
}

build_neon "$FORK_ROOT" "$FORK_BUILD"
build_neon "$OFFICIAL_ROOT" "$OFFICIAL_BUILD"

FORK_ABC="$(printf abc | "$FORK_BUILD/blake3-example")"
OFFICIAL_ABC="$(printf abc | "$OFFICIAL_BUILD/blake3-example")"

[ "$FORK_ABC" = "$EXPECTED_ABC" ] || {
  printf 'fork_kat=FAIL got=%s\n' "$FORK_ABC" >&2
  exit 1
}
[ "$OFFICIAL_ABC" = "$EXPECTED_ABC" ] || {
  printf 'official_kat=FAIL got=%s\n' "$OFFICIAL_ABC" >&2
  exit 1
}

clang $COMMON_FLAGS -fvisibility=hidden \
  -I "$FORK_ROOT/c" \
  "$FORK_ROOT/c/bench_rmr.c" "$FORK_BUILD/libblake3.a" \
  -o "$FORK_BUILD/bench-rmr"

clang $COMMON_FLAGS -fvisibility=hidden \
  -I "$OFFICIAL_ROOT/c" \
  "$FORK_ROOT/c/bench_rmr.c" "$OFFICIAL_BUILD/libblake3.a" \
  -o "$OFFICIAL_BUILD/bench-rmr"

{
  echo "device_arch=$(uname -m)"
  echo "device_model=$(getprop ro.product.model 2>/dev/null || echo TOKEN_VAZIO)"
  echo "android_release=$(getprop ro.build.version.release 2>/dev/null || echo TOKEN_VAZIO)"
  echo "android_sdk=$(getprop ro.build.version.sdk 2>/dev/null || echo TOKEN_VAZIO)"
  echo "android_abi=$(getprop ro.product.cpu.abi 2>/dev/null || echo TOKEN_VAZIO)"
  echo "fork_repo=$(git -C "$FORK_ROOT" remote get-url origin)"
  echo "fork_commit=$(git -C "$FORK_ROOT" rev-parse HEAD)"
  echo "official_repo=$OFFICIAL_REPO"
  echo "official_ref=$OFFICIAL_REF"
  echo "official_commit=$(git -C "$OFFICIAL_ROOT" rev-parse HEAD)"
  echo "compiler=$(clang --version | head -n 1)"
  echo "cmake=$(cmake --version | head -n 1)"
  echo "flags=$COMMON_FLAGS"
  echo "rounds=$ROUNDS"
  echo "iterations_mib=$ITERATIONS_MIB"
  echo "kat_abc=PASS"

  for ((round = 1; round <= ROUNDS; round++)); do
    echo "round=$round"
    if ((round % 2 == 1)); then
      "$OFFICIAL_BUILD/bench-rmr" official "$ITERATIONS_MIB"
      sleep 2
      "$FORK_BUILD/bench-rmr" fork "$ITERATIONS_MIB"
    else
      "$FORK_BUILD/bench-rmr" fork "$ITERATIONS_MIB"
      sleep 2
      "$OFFICIAL_BUILD/bench-rmr" official "$ITERATIONS_MIB"
    fi
    sleep 3
  done
} | tee "$RESULTS"

median_for() {
  local mode="$1"
  awk -F, -v mode="$mode" '$1 == "RESULT" && $2 == mode {print $4}' "$RESULTS" |
    sort -n |
    awk '{v[NR]=$1} END {if (NR == 0) exit 1; if (NR % 2) print v[(NR+1)/2]; else printf "%.6f\n", (v[NR/2]+v[NR/2+1])/2}'
}

OFFICIAL_MEDIAN="$(median_for official)"
FORK_MEDIAN="$(median_for fork)"
RATIO="$(awk -v official="$OFFICIAL_MEDIAN" -v fork="$FORK_MEDIAN" 'BEGIN {printf "%.4f", fork / official}')"
GAIN="$(awk -v official="$OFFICIAL_MEDIAN" -v fork="$FORK_MEDIAN" 'BEGIN {printf "%.2f", ((fork / official) - 1.0) * 100.0}')"
DIGEST_COUNT="$(awk -F, '$1 == "RESULT" {print $6}' "$RESULTS" | sort -u | awk 'END {print NR}')"

[ "$DIGEST_COUNT" -eq 1 ] || {
  echo "digest_equivalence=FAIL" >&2
  exit 1
}

{
  echo "status=PASS"
  echo "comparison=fork_vs_official_same_device_same_compiler_same_flags"
  echo "official_median_MiB_s=$OFFICIAL_MEDIAN"
  echo "fork_median_MiB_s=$FORK_MEDIAN"
  echo "fork_over_official_ratio=${RATIO}x"
  echo "fork_gain_percent=${GAIN}%"
  echo "digest_equivalence=PASS"
  echo "claim_scope=ARMv7_Termux_single_device_single_run_matrix"
} | tee "$SUMMARY"

{
  cat "$SUMMARY"
  echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "fork_commit=$(git -C "$FORK_ROOT" rev-parse HEAD)"
  echo "official_commit=$(git -C "$OFFICIAL_ROOT" rev-parse HEAD)"
  echo "results=$RESULTS"
  echo
  cat "$RESULTS"
} > "$RECEIPT"

sha256sum \
  "$RESULTS" \
  "$SUMMARY" \
  "$RECEIPT" \
  "$FORK_BUILD/libblake3.a" \
  "$OFFICIAL_BUILD/libblake3.a" \
  > "$RUN_DIR/SHA256SUMS"

printf '\nreceipt=%s\n' "$RECEIPT"
printf 'hashes=%s\n' "$RUN_DIR/SHA256SUMS"
