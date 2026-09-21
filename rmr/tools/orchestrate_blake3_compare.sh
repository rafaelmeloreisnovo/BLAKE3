#!/usr/bin/env bash
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-6aab490a26124663329dfd3961b8469f8fdb158b}"
ROUNDS="${ROUNDS:-5}"
ITERATIONS_MIB="${ITERATIONS_MIB:-64}"
JOBS="${JOBS:-2}"
RMR_CC="${RMR_CC:-${CC:-cc}}"
OFFICIAL_CC="${OFFICIAL_CC:-${CC:-cc}}"
WORK_ROOT="${WORK_ROOT:-$ROOT/.rmr-work/blake3-compare}"
RESULT_ROOT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/comparisons}"
STAMP="$(date -u +%Y%m%dT%H%M%SZ)"
RUN_DIR="$RESULT_ROOT/$STAMP"
OFFICIAL_ROOT="$WORK_ROOT/official"
OFFICIAL_BUILD="$WORK_ROOT/build-official"
RMR_BUILD="$WORK_ROOT/build-rmr"
EXPECTED_ABC="6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85"

need() {
  command -v "$1" >/dev/null 2>&1 || { echo "missing_command=$1" >&2; exit 127; }
}
for t in git cmake "$RMR_CC" "$OFFICIAL_CC" awk sort sha256sum date uname; do need "$t"; done

cxx_for() {
  case "$1" in
    *clang*) echo clang++ ;;
    *gcc*) echo g++ ;;
    *) echo c++ ;;
  esac
}

mkdir -p "$WORK_ROOT" "$RUN_DIR"

if [ ! -d "$OFFICIAL_ROOT/.git" ]; then
  git clone --filter=blob:none --no-tags "$OFFICIAL_REPO" "$OFFICIAL_ROOT"
fi
git -C "$OFFICIAL_ROOT" fetch --depth 1 origin "$OFFICIAL_REF"
git -C "$OFFICIAL_ROOT" checkout --detach FETCH_HEAD
OFFICIAL_COMMIT="$(git -C "$OFFICIAL_ROOT" rev-parse HEAD)"
FORK_COMMIT="$(git -C "$ROOT" rev-parse HEAD)"

GEN=()
if command -v ninja >/dev/null 2>&1; then GEN=(-G Ninja); fi

rm -rf "$OFFICIAL_BUILD" "$RMR_BUILD"

cmake -S "$OFFICIAL_ROOT/c" -B "$OFFICIAL_BUILD" "${GEN[@]}"   -DCMAKE_BUILD_TYPE=Release   -DCMAKE_C_COMPILER="$OFFICIAL_CC"   -DCMAKE_CXX_COMPILER="$(cxx_for "$OFFICIAL_CC")"   -DBUILD_SHARED_LIBS=OFF   -DBLAKE3_USE_TBB=OFF   -DBLAKE3_EXAMPLES=ON   >"$RUN_DIR/official-configure.log" 2>&1
cmake --build "$OFFICIAL_BUILD" --parallel "$JOBS"   >"$RUN_DIR/official-build.log" 2>&1

cmake -S "$ROOT/rmr" -B "$RMR_BUILD" "${GEN[@]}"   -DCMAKE_BUILD_TYPE=Release   -DCMAKE_C_COMPILER="$RMR_CC"   -DCMAKE_CXX_COMPILER="$(cxx_for "$RMR_CC")"   -DRMR_BUILD_TESTS=ON   >"$RUN_DIR/rmr-configure.log" 2>&1
cmake --build "$RMR_BUILD" --parallel "$JOBS"   >"$RUN_DIR/rmr-build.log" 2>&1
ctest --test-dir "$RMR_BUILD" --output-on-failure   >"$RUN_DIR/rmr-ctest.log" 2>&1

printf abc > "$RUN_DIR/abc.txt"
OFFICIAL_ABC="$(printf abc | "$OFFICIAL_BUILD/blake3-example")"
RMR_ABC="$("$RMR_BUILD/pai" run --mode cli hash --algo blake3 --file "$RUN_DIR/abc.txt" | awk '{print $1}')"
[ "$OFFICIAL_ABC" = "$EXPECTED_ABC" ] || { echo "official_kat=FAIL" >&2; exit 10; }
[ "$RMR_ABC" = "$EXPECTED_ABC" ] || { echo "rmr_kat=FAIL" >&2; exit 11; }

OFFICIAL_LIB="$(find "$OFFICIAL_BUILD" -type f -name 'libblake3.a' -print -quit)"
[ -n "$OFFICIAL_LIB" ] || { echo "official_static_library=TOKEN_VAZIO" >&2; exit 12; }

"$OFFICIAL_CC" -O3 -std=c11   -I"$OFFICIAL_ROOT/c"   "$ROOT/rmr/benchmark_framework/core/blake3_bench.c"   "$OFFICIAL_LIB"   -o "$OFFICIAL_BUILD/rmr-blake3-bench"

RESULTS="$RUN_DIR/results.csv"
printf 'record,mode,seconds,mib_s,guard,digest\n' > "$RESULTS"

for ((round=1; round<=ROUNDS; round++)); do
  if (( round % 2 == 1 )); then
    "$OFFICIAL_BUILD/rmr-blake3-bench" official "$ITERATIONS_MIB" >> "$RESULTS"
    "$RMR_BUILD/rmr-blake3-bench" rmr "$ITERATIONS_MIB" >> "$RESULTS"
  else
    "$RMR_BUILD/rmr-blake3-bench" rmr "$ITERATIONS_MIB" >> "$RESULTS"
    "$OFFICIAL_BUILD/rmr-blake3-bench" official "$ITERATIONS_MIB" >> "$RESULTS"
  fi
done

DIGEST_COUNT="$(awk -F, '$1=="RESULT"{print $6}' "$RESULTS" | sort -u | awk 'END{print NR+0}')"
[ "$DIGEST_COUNT" -eq 1 ] || { echo "digest_equivalence=FAIL" >&2; exit 20; }

median_for() {
  local mode="$1"
  awk -F, -v m="$mode" '$1=="RESULT" && $2==m {print $4}' "$RESULTS" |
    sort -n |
    awk '{v[NR]=$1} END {if(NR==0) exit 1; if(NR%2) print v[(NR+1)/2]; else printf "%.6f\n",(v[NR/2]+v[NR/2+1])/2}'
}
OFFICIAL_MEDIAN="$(median_for official)"
RMR_MEDIAN="$(median_for rmr)"
RATIO="$(awk -v a="$OFFICIAL_MEDIAN" -v b="$RMR_MEDIAN" 'BEGIN{printf "%.6f",b/a}')"
DELTA="$(awk -v a="$OFFICIAL_MEDIAN" -v b="$RMR_MEDIAN" 'BEGIN{printf "%.3f",((b/a)-1)*100}')"

{
  echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "host=$(uname -a)"
  echo "official_repo=$OFFICIAL_REPO"
  echo "official_ref=$OFFICIAL_REF"
  echo "official_commit=$OFFICIAL_COMMIT"
  echo "fork_commit=$FORK_COMMIT"
  echo "official_compiler=$("$OFFICIAL_CC" --version | head -n1)"
  echo "rmr_compiler=$("$RMR_CC" --version | head -n1)"
  echo "cmake=$(cmake --version | head -n1)"
  echo "rounds=$ROUNDS"
  echo "iterations_mib=$ITERATIONS_MIB"
  echo "kat_abc=PASS"
  echo "digest_equivalence=PASS"
} > "$RUN_DIR/environment.txt"

cat > "$RUN_DIR/summary.json" <<JSON
{
  "state": "EXECUTED_LOCAL_OR_CI",
  "claim_allowed": false,
  "official_commit": "$OFFICIAL_COMMIT",
  "fork_commit": "$FORK_COMMIT",
  "rounds": $ROUNDS,
  "iterations_mib": $ITERATIONS_MIB,
  "official_median_mib_s": $OFFICIAL_MEDIAN,
  "rmr_median_mib_s": $RMR_MEDIAN,
  "rmr_over_official_ratio": $RATIO,
  "rmr_delta_percent": $DELTA,
  "kat_abc": "PASS",
  "digest_equivalence": "PASS",
  "independent_third_party_reproduction": "TOKEN_VAZIO"
}
JSON

cat > "$RUN_DIR/receipt.md" <<RECEIPT
# RMR BLAKE3 comparison receipt

SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM

- official source: $OFFICIAL_REPO
- official commit: $OFFICIAL_COMMIT
- fork commit: $FORK_COMMIT
- official build system: official c/CMakeLists.txt
- RMR build system: rmr/CMakeLists.txt
- common harness: rmr/benchmark_framework/core/blake3_bench.c
- abc KAT: PASS
- digest equivalence across measured runs: PASS
- independent third-party reproduction: TOKEN_VAZIO

Performance numbers in summary.json are local to this recorded environment.
They do not establish universal superiority.
RECEIPT

ARTIFACTS=(
  "$RESULTS"
  "$RUN_DIR/environment.txt"
  "$RUN_DIR/summary.json"
  "$RUN_DIR/receipt.md"
  "$RUN_DIR/official-configure.log"
  "$RUN_DIR/official-build.log"
  "$RUN_DIR/rmr-configure.log"
  "$RUN_DIR/rmr-build.log"
  "$RUN_DIR/rmr-ctest.log"
)

sha256sum "${ARTIFACTS[@]}" > "$RUN_DIR/SHA256SUMS"
: > "$RUN_DIR/BLAKE3SUMS"
for f in "${ARTIFACTS[@]}"; do
  h="$("$RMR_BUILD/pai" run --mode cli hash --algo blake3 --file "$f" | awk '{print $1}')"
  printf '%s  %s\n' "$h" "$f" >> "$RUN_DIR/BLAKE3SUMS"
done

echo "status=PASS"
echo "run_dir=$RUN_DIR"
echo "official_median_mib_s=$OFFICIAL_MEDIAN"
echo "rmr_median_mib_s=$RMR_MEDIAN"
echo "rmr_over_official_ratio=${RATIO}x"
echo "independent_third_party_reproduction=TOKEN_VAZIO"
