#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
# CPU-pinned compiler/cap matrix for BLAKE3_LIKELY policy.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-refs/heads/master}"
ROUNDS="${ROUNDS:-12}"
TARGET_MIB="${TARGET_MIB:-128}"
SIZES="${SIZES:-64 1024 65536 1048576}"
OUT="$ROOT/rmr/benchmark_framework/output/likely-policy-v2"
WORK="$ROOT/.rmr-work/likely-policy-v2"
OFFICIAL_ROOT="$WORK/official"
EXPECTED_ABC="6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85"

[ "$OFFICIAL_REPO" = "https://github.com/BLAKE3-team/BLAKE3.git" ] || exit 2
[ "$OFFICIAL_REF" = "refs/heads/master" ] || exit 2
case "$ROUNDS" in ''|*[!0-9]*) exit 2;; esac
[ $((ROUNDS % 3)) -eq 0 ] || { echo "rounds_must_be_multiple_of_3" >&2; exit 2; }

need() { command -v "$1" >/dev/null 2>&1 || { echo "missing_command=$1" >&2; exit 127; }; }
for t in git gcc g++ clang clang++ cmake ninja python3 awk sha256sum taskset stat nproc; do need "$t"; done
rm -rf "$OUT" "$WORK"
mkdir -p "$OUT" "$WORK" "$OUT/scheduler"

AFFINITY="$(taskset -pc $$ | awk -F: '{gsub(/[[:space:]]/,"",$2); print $2}')"
FIRST_TOKEN="${AFFINITY%%,*}"
PIN_CPU="${FIRST_TOKEN%%-*}"
case "$PIN_CPU" in ''|*[!0-9]*) echo "pin_cpu=TOKEN_VAZIO affinity=$AFFINITY" >&2; exit 3;; esac
taskset -c "$PIN_CPU" true

OFFICIAL_SHA="$(git ls-remote "$OFFICIAL_REPO" "$OFFICIAL_REF" | awk 'NR==1{print $1}')"
printf '%s' "$OFFICIAL_SHA" | grep -Eq '^[0-9a-f]{40}$' || exit 4
git init -q "$OFFICIAL_ROOT"
git -C "$OFFICIAL_ROOT" remote add origin "$OFFICIAL_REPO"
git -C "$OFFICIAL_ROOT" fetch -q --no-tags --depth=1 origin "$OFFICIAL_SHA"
git -C "$OFFICIAL_ROOT" checkout -q --detach FETCH_HEAD
[ "$(git -C "$OFFICIAL_ROOT" rev-parse HEAD)" = "$OFFICIAL_SHA" ] || exit 5

cp -a "$ROOT/c" "$WORK/fork"
cp -a "$ROOT/c" "$WORK/fork_no_likely"
python3 - "$WORK/fork_no_likely/blake3_dispatch.c" <<'PY'
from pathlib import Path
import sys
p=Path(sys.argv[1]); s=p.read_text(encoding="utf-8")
old="#define BLAKE3_LIKELY(x) __builtin_expect(!!(x), 1)"
if s.count(old)!=1: raise SystemExit("likely_replacement=FAIL")
p.write_text(s.replace(old,"#define BLAKE3_LIKELY(x) (x)"),encoding="utf-8")
PY

declare -A SRC
SRC[official]="$OFFICIAL_ROOT/c"
SRC[fork]="$WORK/fork"
SRC[fork_no_likely]="$WORK/fork_no_likely"
VARIANTS=(official fork fork_no_likely)
COMPILERS=(gcc clang)
CAPS=(sse2 sse41 avx2 auto)

compiler_cxx() { case "$1" in gcc) echo g++;; clang) echo clang++;; *) return 2;; esac; }
cap_flags() {
  case "$1" in
    sse2) printf '%s\n' "-DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512" ;;
    sse41) printf '%s\n' "-DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512" ;;
    avx2) printf '%s\n' "-DBLAKE3_NO_AVX512" ;;
    auto) printf '%s\n' "" ;;
    *) return 2 ;;
  esac
}

sched_snapshot() {
  local tag="$1" f="$OUT/scheduler/$tag.txt"
  {
    echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "affinity=$AFFINITY"
    echo "pin_cpu=$PIN_CPU"
    echo "loadavg=$(cat /proc/loadavg 2>/dev/null || true)"
    echo "cpu_total=$(grep '^cpu ' /proc/stat 2>/dev/null || true)"
    echo "ctxt=$(awk '/^ctxt /{print $2}' /proc/stat 2>/dev/null || true)"
    echo "processes=$(awk '/^processes /{print $2}' /proc/stat 2>/dev/null || true)"
    echo "pressure_cpu=$(tr '\n' ';' </proc/pressure/cpu 2>/dev/null || true)"
    awk -v target="$PIN_CPU" '
      /^processor[[:space:]]*:/ {p=$3; keep=(p==target)}
      keep && /^(model name|cpu MHz|cpu family|model[[:space:]]|stepping)[[:space:]]*:/ {gsub(/^[[:space:]]+/,""); print "pinned_" $0}
    ' /proc/cpuinfo 2>/dev/null || true
  } > "$f"
}

{
  echo "schema=RMR-LIKELY-POLICY-VM-V2"
  echo "runner_os=${RUNNER_OS:-TOKEN_VAZIO}"
  echo "runner_arch=${RUNNER_ARCH:-TOKEN_VAZIO}"
  echo "image_os=${ImageOS:-TOKEN_VAZIO}"
  echo "image_version=${ImageVersion:-TOKEN_VAZIO}"
  echo "uname=$(uname -a)"
  echo "nproc=$(nproc)"
  echo "affinity=$AFFINITY"
  echo "pin_cpu=$PIN_CPU"
  echo "cpu_model=$(awk -F: '/model name/{gsub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo 2>/dev/null || true)"
  echo "gcc=$(gcc --version | head -n1)"
  echo "clang=$(clang --version | head -n1)"
  echo "cmake=$(cmake --version | head -n1)"
} > "$OUT/vm-fingerprint.txt"

printf 'compiler,cap,variant,artifact,sha256,size_bytes,simd_degree\n' > "$OUT/object_hashes.csv"
printf 'compiler,cap,variant,size_bytes,round,position,iterations,seconds,ns_per_op,mib_s,digest\n' > "$OUT/results.csv"

iterations_for() {
  python3 - "$1" "$TARGET_MIB" <<'PY'
import math,sys
print(max(1,math.ceil(int(sys.argv[2])*1024*1024/int(sys.argv[1]))))
PY
}

for compiler in "${COMPILERS[@]}"; do
  cxx="$(compiler_cxx "$compiler")"
  for cap in "${CAPS[@]}"; do
    sched_snapshot "${compiler}-${cap}-before"
    defs="$(cap_flags "$cap")"
    declare -A BENCH DEGREE
    for v in "${VARIANTS[@]}"; do
      src="${SRC[$v]}"
      build="$WORK/build-${compiler}-${cap}-${v}"
      rm -rf "$build"
      cflags="-O3"; [ -z "$defs" ] || cflags="$cflags $defs"
      cmake -S "$src" -B "$build" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER="$compiler" \
        -DCMAKE_CXX_COMPILER="$cxx" \
        -DCMAKE_C_FLAGS="$cflags" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DBUILD_SHARED_LIBS=OFF \
        -DBLAKE3_USE_TBB=OFF \
        -DBLAKE3_FETCH_TBB=OFF \
        -DBLAKE3_EXAMPLES=OFF \
        -DBLAKE3_SIMD_TYPE=amd64-asm \
        > "$OUT/${compiler}-${cap}-${v}-configure.log" 2>&1
      cmake --build "$build" --parallel 2 > "$OUT/${compiler}-${cap}-${v}-build.log" 2>&1
      lib="$(find "$build" -type f -name 'libblake3.a' -print -quit)"
      [ -n "$lib" ] || exit 10
      "$compiler" -O3 -std=c11 -I"$src" -c "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" -o "$build/bench.o"
      "$compiler" "$build/bench.o" "$lib" -o "$build/bench"
      "$compiler" -O3 -std=c11 -I"$src" "$ROOT/rmr/upstream_validation/blake3_backend_probe.c" "$lib" -o "$build/probe"
      probe="$("$build/probe")"
      abc="$(printf '%s\n' "$probe" | awk -F= '$1=="abc"{print $2}')"
      degree="$(printf '%s\n' "$probe" | awk -F= '$1=="simd_degree"{print $2}')"
      [ "$abc" = "$EXPECTED_ABC" ] || { echo "KAT=FAIL $compiler $cap $v" >&2; exit 11; }
      case "$cap" in sse2|sse41) [ "$degree" = "4" ] || exit 12;; avx2) [ "$degree" = "8" ] || exit 12;; auto) [ "$degree" -ge 4 ] || exit 12;; esac
      BENCH[$v]="$build/bench"; DEGREE[$v]="$degree"
      for spec in \
        "libblake3.a|$lib" \
        "bench.o|$build/bench.o" \
        "dispatch.o|$build/CMakeFiles/blake3.dir/blake3_dispatch.c.o"; do
        IFS='|' read -r art p <<<"$spec"
        sha="$(sha256sum "$p" | awk '{print $1}')"
        sz="$(stat -c%s "$p")"
        printf '%s,%s,%s,%s,%s,%s,%s\n' "$compiler" "$cap" "$v" "$art" "$sha" "$sz" "$degree" >> "$OUT/object_hashes.csv"
      done
    done

    for size_bytes in $SIZES; do
      iters="$(iterations_for "$size_bytes")"
      for v in "${VARIANTS[@]}"; do taskset -c "$PIN_CPU" "${BENCH[$v]}" warmup "$size_bytes" "$iters" >/dev/null; done
      for ((round=1; round<=ROUNDS; round++)); do
        shift=$(( (round - 1) % 3 ))
        for ((position=0; position<3; position++)); do
          idx=$(( (position + shift) % 3 ))
          v="${VARIANTS[$idx]}"
          line="$(taskset -c "$PIN_CPU" "${BENCH[$v]}" "$v" "$size_bytes" "$iters")"
          sec="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $5}')"
          ns="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $6}')"
          mib="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $7}')"
          digest="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $9}')"
          printf '%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n' "$compiler" "$cap" "$v" "$size_bytes" "$round" "$position" "$iters" "$sec" "$ns" "$mib" "$digest" >> "$OUT/results.csv"
        done
      done
    done
    sched_snapshot "${compiler}-${cap}-after"
    rm -rf "$WORK/build-${compiler}-${cap}-"*
    unset BENCH DEGREE
  done
done

python3 "$ROOT/rmr/upstream_validation/analyze_likely_policy_matrix_v2.py" \
  --csv "$OUT/results.csv" --objects "$OUT/object_hashes.csv" \
  --json "$OUT/summary.json" --md "$OUT/summary.md"

{
  echo "schema=RMR-LIKELY-POLICY-MATRIX-V2"
  echo "official_repo=$OFFICIAL_REPO"
  echo "official_sha=$OFFICIAL_SHA"
  echo "fork_sha=$(git -C "$ROOT" rev-parse HEAD)"
  echo "compilers=gcc clang"
  echo "caps=sse2 sse41 avx2 auto"
  echo "rounds=$ROUNDS"
  echo "rotation=LATIN_CYCLIC_3"
  echo "cpu_pin=$PIN_CPU"
  echo "affinity=$AFFINITY"
  echo "target_mib=$TARGET_MIB"
  echo "sizes=$SIZES"
  echo "claim_allowed=false"
  echo "policy_decision=PENDING_EVIDENCE"
} > "$OUT/receipt.txt"

(
  cd "$OUT"
  find . -type f ! -name SHA256SUMS.txt -print0 | sort -z | xargs -0 sha256sum
) > "$OUT/SHA256SUMS.txt"
echo "RMR_LIKELY_POLICY_V2=PASS"
cat "$OUT/summary.md"
