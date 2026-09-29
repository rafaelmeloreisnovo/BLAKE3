#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
# Paired causal experiment for the GCC + SSE4.1 anomaly observed by RMR V4.
# SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-refs/heads/master}"
ROUNDS="${ROUNDS:-15}"
TARGET_MIB="${TARGET_MIB:-128}"
SIZES="${SIZES:-64 1024 65536 1048576}"
OUT="$ROOT/rmr/benchmark_framework/output/gcc-sse41-causal-v1"
WORK="$ROOT/.rmr-work/gcc-sse41-causal-v1"
OFFICIAL_ROOT="$WORK/official"
EXPECTED_ABC="6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85"

[ "$OFFICIAL_REPO" = "https://github.com/BLAKE3-team/BLAKE3.git" ] || { echo "official_repo_authority=FAIL" >&2; exit 2; }
[ "$OFFICIAL_REF" = "refs/heads/master" ] || { echo "official_ref_authority=FAIL" >&2; exit 2; }
case "$ROUNDS" in ''|*[!0-9]*) exit 2;; esac
[ "$ROUNDS" -ge 5 ] || { echo "rounds_too_small" >&2; exit 2; }

need() { command -v "$1" >/dev/null 2>&1 || { echo "missing_command=$1" >&2; exit 127; }; }
for t in git gcc g++ cmake ninja python3 awk sha256sum objdump nm readelf size stat nproc; do need "$t"; done
rm -rf "$OUT" "$WORK"
mkdir -p "$OUT" "$WORK" "$OUT/binaries" "$OUT/disassembly" "$OUT/compile-commands"

OFFICIAL_SHA="$(git ls-remote "$OFFICIAL_REPO" "$OFFICIAL_REF" | awk 'NR==1{print $1}')"
printf '%s' "$OFFICIAL_SHA" | grep -Eq '^[0-9a-f]{40}$' || { echo "official_sha=FAIL" >&2; exit 3; }
git init -q "$OFFICIAL_ROOT"
git -C "$OFFICIAL_ROOT" remote add origin "$OFFICIAL_REPO"
git -C "$OFFICIAL_ROOT" fetch -q --no-tags --depth=1 origin "$OFFICIAL_SHA"
git -C "$OFFICIAL_ROOT" checkout -q --detach FETCH_HEAD
[ "$(git -C "$OFFICIAL_ROOT" rev-parse HEAD)" = "$OFFICIAL_SHA" ] || exit 4

cp -a "$ROOT/c" "$WORK/fork"
cp -a "$ROOT/c" "$WORK/fork_no_likely"
cp -a "$ROOT/c" "$WORK/fork_no_restrict"
cp -a "$ROOT/c" "$WORK/fork_no_hints"

python3 - "$WORK" <<'PY'
from pathlib import Path
import sys
w=Path(sys.argv[1])
def replace_once(path, old, new):
    p=Path(path); s=p.read_text(encoding="utf-8")
    n=s.count(old)
    if n != 1:
        raise SystemExit(f"replacement_count=FAIL path={p} count={n}")
    p.write_text(s.replace(old,new), encoding="utf-8")
for name in ("fork_no_likely","fork_no_hints"):
    replace_once(w/name/"blake3_dispatch.c",
                 "#define BLAKE3_LIKELY(x) __builtin_expect(!!(x), 1)",
                 "#define BLAKE3_LIKELY(x) (x)")
for name in ("fork_no_restrict","fork_no_hints"):
    replace_once(w/name/"blake3.h",
                 "#define BLAKE3_RESTRICT __restrict__",
                 "#define BLAKE3_RESTRICT")
PY

{
  echo "schema=RMR-GCC-SSE41-VM-V1"
  echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  echo "runner_os=${RUNNER_OS:-TOKEN_VAZIO}"
  echo "runner_arch=${RUNNER_ARCH:-TOKEN_VAZIO}"
  echo "image_os=${ImageOS:-TOKEN_VAZIO}"
  echo "image_version=${ImageVersion:-TOKEN_VAZIO}"
  echo "uname=$(uname -a)"
  echo "nproc=$(nproc)"
  echo "cpu_model=$(awk -F: '/model name/{gsub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo 2>/dev/null || true)"
  echo "cpu_flags=$(awk -F: '/^flags/{gsub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo 2>/dev/null || true)"
  echo "gcc=$(gcc --version | head -n1)"
  echo "cmake=$(cmake --version | head -n1)"
} > "$OUT/vm-fingerprint.txt"

declare -A SRC BENCH LIB
SRC[official]="$OFFICIAL_ROOT/c"
SRC[fork]="$WORK/fork"
SRC[fork_no_likely]="$WORK/fork_no_likely"
SRC[fork_no_restrict]="$WORK/fork_no_restrict"
SRC[fork_no_hints]="$WORK/fork_no_hints"
VARIANTS=(official fork fork_no_likely fork_no_restrict fork_no_hints)

printf 'variant,artifact,path,sha256,size_bytes\n' > "$OUT/object_hashes.csv"

build_variant() {
  local v="$1" src="${SRC[$1]}" build="$WORK/build-$1"
  rm -rf "$build"
  cmake -S "$src" -B "$build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_FLAGS="-O3 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -DBLAKE3_USE_TBB=OFF \
    -DBLAKE3_FETCH_TBB=OFF \
    -DBLAKE3_EXAMPLES=OFF \
    -DBLAKE3_SIMD_TYPE=amd64-asm \
    > "$OUT/$v-configure.log" 2>&1
  cmake --build "$build" --parallel 2 > "$OUT/$v-build.log" 2>&1
  local lib; lib="$(find "$build" -type f -name 'libblake3.a' -print -quit)"
  [ -n "$lib" ] || { echo "lib=TOKEN_VAZIO variant=$v" >&2; exit 10; }

  gcc -O3 -std=c11 -I"$src" -c "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" -o "$build/bench.o"
  gcc "$build/bench.o" "$lib" -o "$build/bench"
  gcc -O3 -std=c11 -I"$src" "$ROOT/rmr/upstream_validation/blake3_backend_probe.c" "$lib" -o "$build/probe"
  "$build/probe" > "$OUT/$v-probe.txt"
  local abc degree
  abc="$(awk -F= '$1=="abc"{print $2}' "$OUT/$v-probe.txt")"
  degree="$(awk -F= '$1=="simd_degree"{print $2}' "$OUT/$v-probe.txt")"
  [ "$abc" = "$EXPECTED_ABC" ] || { echo "KAT=FAIL variant=$v" >&2; exit 11; }
  [ "$degree" = "4" ] || { echo "SIMD_DEGREE=FAIL variant=$v degree=$degree" >&2; exit 12; }

  cp "$lib" "$OUT/binaries/$v-libblake3.a"
  cp "$build/bench.o" "$OUT/binaries/$v-bench.o"
  cp "$build/compile_commands.json" "$OUT/compile-commands/$v.json"
  nm -n "$lib" > "$OUT/disassembly/$v-nm.txt"
  size -A "$lib" > "$OUT/disassembly/$v-size.txt"
  readelf -SW "$build/CMakeFiles/blake3.dir/blake3_dispatch.c.o" > "$OUT/disassembly/$v-dispatch-sections.txt"
  objdump -drwC "$build/CMakeFiles/blake3.dir/blake3_dispatch.c.o" > "$OUT/disassembly/$v-dispatch-objdump.txt"

  local p art sha sz
  for spec in \
    "libblake3.a|$lib" \
    "bench.o|$build/bench.o" \
    "blake3.c.o|$build/CMakeFiles/blake3.dir/blake3.c.o" \
    "blake3_dispatch.c.o|$build/CMakeFiles/blake3.dir/blake3_dispatch.c.o" \
    "blake3_portable.c.o|$build/CMakeFiles/blake3.dir/blake3_portable.c.o" \
    "blake3_sse41.S.o|$build/CMakeFiles/blake3.dir/blake3_sse41_x86-64_unix.S.o"; do
    IFS='|' read -r art p <<<"$spec"
    [ -f "$p" ] || { echo "object=TOKEN_VAZIO variant=$v artifact=$art" >&2; exit 13; }
    sha="$(sha256sum "$p" | awk '{print $1}')"
    sz="$(stat -c%s "$p")"
    printf '%s,%s,%s,%s,%s\n' "$v" "$art" "$p" "$sha" "$sz" >> "$OUT/object_hashes.csv"
  done
  BENCH[$v]="$build/bench"
  LIB[$v]="$lib"
}

for v in "${VARIANTS[@]}"; do build_variant "$v"; done

iterations_for() {
  python3 - "$1" "$TARGET_MIB" <<'PY'
import math,sys
print(max(1, math.ceil(int(sys.argv[2])*1024*1024/int(sys.argv[1]))))
PY
}

printf 'variant,size_bytes,round,position,iterations,seconds,ns_per_op,mib_s,digest\n' > "$OUT/results.csv"
for size_bytes in $SIZES; do
  iters="$(iterations_for "$size_bytes")"
  for v in "${VARIANTS[@]}"; do "${BENCH[$v]}" warmup "$size_bytes" "$iters" >/dev/null; done
  for ((round=1; round<=ROUNDS; round++)); do
    shift=$(( (round - 1) % ${#VARIANTS[@]} ))
    for ((position=0; position<${#VARIANTS[@]}; position++)); do
      idx=$(( (position + shift) % ${#VARIANTS[@]} ))
      v="${VARIANTS[$idx]}"
      line="$("${BENCH[$v]}" "$v" "$size_bytes" "$iters")"
      sec="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $5}')"
      ns="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $6}')"
      mib="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $7}')"
      digest="$(printf '%s\n' "$line" | awk -F, '$1=="SIZE_RESULT"{print $9}')"
      printf '%s,%s,%s,%s,%s,%s,%s,%s,%s\n' "$v" "$size_bytes" "$round" "$position" "$iters" "$sec" "$ns" "$mib" "$digest" >> "$OUT/results.csv"
    done
  done
done

python3 "$ROOT/rmr/upstream_validation/analyze_gcc_sse41_causal_v1.py" \
  --csv "$OUT/results.csv" \
  --objects "$OUT/object_hashes.csv" \
  --json "$OUT/summary.json" \
  --md "$OUT/summary.md"

{
  echo "schema=RMR-GCC-SSE41-CAUSAL-V1"
  echo "official_repo=$OFFICIAL_REPO"
  echo "official_sha=$OFFICIAL_SHA"
  echo "fork_sha=$(git -C "$ROOT" rev-parse HEAD)"
  echo "compiler=$(gcc --version | head -n1)"
  echo "simd_type=amd64-asm"
  echo "cap=sse41"
  echo "cflags=-O3 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512"
  echo "rounds=$ROUNDS"
  echo "rotation=LATIN_CYCLIC_5"
  echo "target_mib=$TARGET_MIB"
  echo "sizes=$SIZES"
  echo "kat=PASS_ALL_VARIANTS"
  echo "simd_degree=4_ALL_VARIANTS"
  echo "claim_allowed=false"
  echo "causal_promotion=OBSERVED_UNPROMOTED"
} > "$OUT/receipt.txt"

(
  cd "$OUT"
  find . -type f ! -name SHA256SUMS.txt -print0 | sort -z | xargs -0 sha256sum
) > "$OUT/SHA256SUMS.txt"
echo "RMR_GCC_SSE41_CAUSAL_V1=PASS"
cat "$OUT/summary.md"
