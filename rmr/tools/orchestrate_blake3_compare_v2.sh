#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# Common-harness comparison of current official BLAKE3 vs this fork.
# This measures core library behavior only. It does not measure the full RMR pipeline.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-6aab490a26124663329dfd3961b8469f8fdb158b}"
CC_BIN="${CC:-clang}"
CXX_BIN="${CXX:-clang++}"
ROUNDS="${ROUNDS:-9}"
TARGET_MIB="${TARGET_MIB:-128}"
SIZES="${SIZES:-64 256 1024 4096 65536 1048576 16777216}"
JOBS="${JOBS:-2}"
WORK_ROOT="${WORK_ROOT:-$ROOT/.rmr-work/blake3-compare-v2}"
RESULT_ROOT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/comparisons-v2}"
STAMP="$(date -u +%Y%m%dT%H%M%SZ)"
RUN_DIR="$RESULT_ROOT/$STAMP"
OFFICIAL_ROOT="$WORK_ROOT/official"
OFFICIAL_BUILD="$WORK_ROOT/build-official"
FORK_BUILD="$WORK_ROOT/build-fork"
EXPECTED_ABC="6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85"

need() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "missing_command=$1" >&2
    exit 127
  }
}

for t in git cmake "$CC_BIN" "$CXX_BIN" python3 awk sha256sum uname date; do
  need "$t"
done

mkdir -p "$WORK_ROOT" "$RUN_DIR"

if [ ! -d "$OFFICIAL_ROOT/.git" ]; then
  git clone --filter=blob:none --no-tags "$OFFICIAL_REPO" "$OFFICIAL_ROOT"
fi

git -C "$OFFICIAL_ROOT" fetch --depth 1 origin "$OFFICIAL_REF"
git -C "$OFFICIAL_ROOT" checkout --detach FETCH_HEAD

OFFICIAL_COMMIT="$(git -C "$OFFICIAL_ROOT" rev-parse HEAD)"
FORK_COMMIT="$(git -C "$ROOT" rev-parse HEAD)"

GEN=()
if command -v ninja >/dev/null 2>&1; then
  GEN=(-G Ninja)
fi

rm -rf "$OFFICIAL_BUILD" "$FORK_BUILD"

COMMON_CMAKE=(
  "${GEN[@]}"
  -DCMAKE_BUILD_TYPE=Release
  -DCMAKE_C_COMPILER="$CC_BIN"
  -DCMAKE_CXX_COMPILER="$CXX_BIN"
  -DBUILD_SHARED_LIBS=OFF
  -DBLAKE3_USE_TBB=OFF
  -DBLAKE3_EXAMPLES=ON
)

cmake -S "$OFFICIAL_ROOT/c" -B "$OFFICIAL_BUILD" "${COMMON_CMAKE[@]}"   >"$RUN_DIR/official-configure.log" 2>&1
cmake --build "$OFFICIAL_BUILD" --parallel "$JOBS"   >"$RUN_DIR/official-build.log" 2>&1

cmake -S "$ROOT/c" -B "$FORK_BUILD" "${COMMON_CMAKE[@]}"   >"$RUN_DIR/fork-configure.log" 2>&1
cmake --build "$FORK_BUILD" --parallel "$JOBS"   >"$RUN_DIR/fork-build.log" 2>&1

OFFICIAL_LIB="$(find "$OFFICIAL_BUILD" -type f -name 'libblake3.a' -print -quit)"
FORK_LIB="$(find "$FORK_BUILD" -type f -name 'libblake3.a' -print -quit)"

[ -n "$OFFICIAL_LIB" ] || { echo "official_static_library=TOKEN_VAZIO" >&2; exit 12; }
[ -n "$FORK_LIB" ] || { echo "fork_static_library=TOKEN_VAZIO" >&2; exit 13; }

"$CC_BIN" -O3 -std=c11   -I"$OFFICIAL_ROOT/c"   "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c"   "$OFFICIAL_LIB"   -o "$OFFICIAL_BUILD/blake3-size-bench"

"$CC_BIN" -O3 -std=c11   -I"$ROOT/c"   "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c"   "$FORK_LIB"   -o "$FORK_BUILD/blake3-size-bench"

OFFICIAL_ABC="$(printf abc | "$OFFICIAL_BUILD/blake3-example")"
FORK_ABC="$(printf abc | "$FORK_BUILD/blake3-example")"
[ "$OFFICIAL_ABC" = "$EXPECTED_ABC" ] || { echo "official_kat=FAIL" >&2; exit 20; }
[ "$FORK_ABC" = "$EXPECTED_ABC" ] || { echo "fork_kat=FAIL" >&2; exit 21; }

iterations_for() {
  local size="$1"
  python3 - "$size" "$TARGET_MIB" <<'PY'
import math, sys
size = int(sys.argv[1])
target_mib = int(sys.argv[2])
target_bytes = target_mib * 1024 * 1024
print(max(1, math.ceil(target_bytes / size)))
PY
}

RAW="$RUN_DIR/results.csv"
printf 'mode,size_bytes,round,iterations,seconds,ns_per_op,mib_s,guard,digest\n' > "$RAW"

append_result() {
  local bin="$1"
  local mode="$2"
  local size="$3"
  local iterations="$4"
  local round="$5"
  local line

  line="$("$bin" "$mode" "$size" "$iterations")"
  printf '%s\n' "$line" | awk -F, -v round="$round" '
    $1=="SIZE_RESULT" {
      printf "%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
        $2,$3,round,$4,$5,$6,$7,$8,$9
    }' >> "$RAW"
}

for size in $SIZES; do
  iterations="$(iterations_for "$size")"

  # Warm both paths with the same work before timed pairs.
  "$OFFICIAL_BUILD/blake3-size-bench" warmup "$size" "$iterations" >/dev/null
  "$FORK_BUILD/blake3-size-bench" warmup "$size" "$iterations" >/dev/null

  for ((round=1; round<=ROUNDS; round++)); do
    if (( round % 2 == 1 )); then
      append_result "$OFFICIAL_BUILD/blake3-size-bench" official "$size" "$iterations" "$round"
      append_result "$FORK_BUILD/blake3-size-bench" fork "$size" "$iterations" "$round"
    else
      append_result "$FORK_BUILD/blake3-size-bench" fork "$size" "$iterations" "$round"
      append_result "$OFFICIAL_BUILD/blake3-size-bench" official "$size" "$iterations" "$round"
    fi
  done
done

python3 "$ROOT/rmr/benchmark_framework/simperf/analyze_blake3_compare_v2.py"   --csv "$RAW"   --out "$RUN_DIR/summary.json"   > "$RUN_DIR/analysis.txt"

{
  echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "host=$(uname -a)"
  echo "official_repo=$OFFICIAL_REPO"
  echo "official_ref=$OFFICIAL_REF"
  echo "official_commit=$OFFICIAL_COMMIT"
  echo "fork_commit=$FORK_COMMIT"
  echo "compiler=$("$CC_BIN" --version | head -n1)"
  echo "cmake=$(cmake --version | head -n1)"
  echo "rounds=$ROUNDS"
  echo "target_mib_per_measurement=$TARGET_MIB"
  echo "sizes=$SIZES"
  echo "official_kat=PASS"
  echo "fork_kat=PASS"
} > "$RUN_DIR/environment.txt"

cat > "$RUN_DIR/receipt.md" <<RECEIPT
# RMR BLAKE3 upstream-vs-fork comparison V2

SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM

- official repository: $OFFICIAL_REPO
- official commit: $OFFICIAL_COMMIT
- fork commit: $FORK_COMMIT
- common benchmark source: rmr/benchmark_framework/core/blake3_size_bench.c
- same compiler driver: $CC_BIN
- same CMake release profile
- TBB disabled on both sides
- alternating execution order by round
- per-size warmup on both sides
- abc KAT: PASS on both sides
- performance classification: see summary.json
- independent third-party reproduction: TOKEN_VAZIO

This experiment compares the fork C core against the current official C core.
It does not measure RMR orchestration, I/O, metadata, custody, or UI.
RECEIPT

ARTIFACTS=(
  "$RAW"
  "$RUN_DIR/summary.json"
  "$RUN_DIR/analysis.txt"
  "$RUN_DIR/environment.txt"
  "$RUN_DIR/receipt.md"
  "$RUN_DIR/official-configure.log"
  "$RUN_DIR/official-build.log"
  "$RUN_DIR/fork-configure.log"
  "$RUN_DIR/fork-build.log"
)

sha256sum "${ARTIFACTS[@]}" > "$RUN_DIR/SHA256SUMS"

echo "status=PASS"
echo "run_dir=$RUN_DIR"
cat "$RUN_DIR/analysis.txt"
echo "independent_third_party_reproduction=TOKEN_VAZIO"
