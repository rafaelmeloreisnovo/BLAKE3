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
FORK_BUILD="$WORK_ROOT/build-fork-neon-strict"
OFFICIAL_BUILD="$WORK_ROOT/build-official-neon-strict"
RESULTS="$RUN_DIR/results.csv"
SUMMARY="$RUN_DIR/summary.txt"
RECEIPT="$RUN_DIR/receipt.txt"
EXPECTED_ABC="6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85"

# Diagnostics are a mandatory gate. They do not directly remove symbols, but
# -Werror prevents warning-bearing code from entering the measured build.
DIAGNOSTIC_FLAGS="-Wall -Wextra -Wpedantic -Werror"

# Code-generation contract shared by fork and official upstream.
CODEGEN_FLAGS="-O3 -DNDEBUG -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=softfp -fvisibility=hidden -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables -fno-unwind-tables"
NEON_FLAGS="-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=softfp"

# Linker contract: discard unreachable sections, omit build-id metadata and
# prevent archive members from being promoted into the dynamic export surface.
LINK_FLAGS="-Wl,--gc-sections -Wl,--build-id=none -Wl,--exclude-libs,ALL"
STRICT_C_FLAGS="$DIAGNOSTIC_FLAGS $CODEGEN_FLAGS"

require() {
  command -v "$1" >/dev/null 2>&1 || {
    printf 'missing_command=%s\n' "$1" >&2
    exit 127
  }
}

for tool in git cmake ninja clang awk sort sha256sum uname date tee nm readelf size; do
  require "$tool"
done

OBJDUMP=""
for candidate in llvm-objdump objdump; do
  if command -v "$candidate" >/dev/null 2>&1; then
    OBJDUMP="$candidate"
    break
  fi
done
[ -n "$OBJDUMP" ] || { echo "missing_command=llvm-objdump_or_objdump" >&2; exit 127; }

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

build_neon_strict() {
  local label="$1"
  local source_root="$2"
  local build_root="$3"
  local configure_log="$RUN_DIR/${label}-configure.log"
  local build_log="$RUN_DIR/${label}-build.log"

  rm -rf "$build_root"

  # Do not suppress CMake author/deprecation warnings. Compiler diagnostics are
  # elevated to errors through both flags and CMAKE_COMPILE_WARNING_AS_ERROR.
  cmake -S "$source_root/c" -B "$build_root" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_C_FLAGS_RELEASE="$STRICT_C_FLAGS" \
    -DCMAKE_EXE_LINKER_FLAGS="$LINK_FLAGS" \
    -DCMAKE_SHARED_LINKER_FLAGS="$LINK_FLAGS" \
    -DCMAKE_COMPILE_WARNING_AS_ERROR=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -DBLAKE3_USE_TBB=OFF \
    -DBLAKE3_EXAMPLES=ON \
    -DBLAKE3_SIMD_TYPE=neon-intrinsics \
    -DBLAKE3_CFLAGS_NEON="$NEON_FLAGS" \
    2>&1 | tee "$configure_log"

  cmake --build "$build_root" --parallel "$JOBS" --verbose \
    2>&1 | tee "$build_log"

  grep -F -- "-Wall" "$build_root/compile_commands.json" >/dev/null
  grep -F -- "-Wextra" "$build_root/compile_commands.json" >/dev/null
  grep -F -- "-Wpedantic" "$build_root/compile_commands.json" >/dev/null
  grep -F -- "-Werror" "$build_root/compile_commands.json" >/dev/null
  grep -F -- "-O3" "$build_root/compile_commands.json" >/dev/null
  grep -F -- "-ffunction-sections" "$build_root/compile_commands.json" >/dev/null
  grep -F -- "-fdata-sections" "$build_root/compile_commands.json" >/dev/null
  grep -F -- "-fvisibility=hidden" "$build_root/compile_commands.json" >/dev/null
}

build_neon_strict fork "$FORK_ROOT" "$FORK_BUILD"
build_neon_strict official "$OFFICIAL_ROOT" "$OFFICIAL_BUILD"

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

# The same benchmark source is linked against each static library. The loop is
# benchmark harness work only; it is not added to either BLAKE3 implementation.
clang $STRICT_C_FLAGS \
  -I "$FORK_ROOT/c" \
  "$FORK_ROOT/c/bench_rmr.c" "$FORK_BUILD/libblake3.a" \
  $LINK_FLAGS \
  -o "$FORK_BUILD/bench-rmr"

clang $STRICT_C_FLAGS \
  -I "$OFFICIAL_ROOT/c" \
  "$FORK_ROOT/c/bench_rmr.c" "$OFFICIAL_BUILD/libblake3.a" \
  $LINK_FLAGS \
  -o "$OFFICIAL_BUILD/bench-rmr"

# Preserve symbol tables and disassembly as evidence. No source is modified.
for label in fork official; do
  if [ "$label" = fork ]; then
    build="$FORK_BUILD"
  else
    build="$OFFICIAL_BUILD"
  fi

  nm -A -g --defined-only "$build/libblake3.a" \
    > "$RUN_DIR/${label}-archive-defined-global-symbols.txt"
  nm -D -g --defined-only "$build/bench-rmr" \
    > "$RUN_DIR/${label}-dynamic-defined-symbols.txt" 2>/dev/null || true
  nm -u "$build/bench-rmr" \
    > "$RUN_DIR/${label}-executable-undefined-symbols.txt" || true
  readelf -Ws "$build/bench-rmr" \
    > "$RUN_DIR/${label}-executable-symbol-table.txt"
  readelf -SW "$build/bench-rmr" \
    > "$RUN_DIR/${label}-executable-sections.txt"
  size -A "$build/bench-rmr" \
    > "$RUN_DIR/${label}-executable-size.txt"
  "$OBJDUMP" -d "$build/libblake3.a" \
    > "$RUN_DIR/${label}-archive-disassembly.txt"
done

FORK_GLOBAL_SYMBOLS="$(awk 'NF {count++} END {print count+0}' "$RUN_DIR/fork-archive-defined-global-symbols.txt")"
OFFICIAL_GLOBAL_SYMBOLS="$(awk 'NF {count++} END {print count+0}' "$RUN_DIR/official-archive-defined-global-symbols.txt")"
FORK_DYNAMIC_EXPORTS="$(awk 'NF {count++} END {print count+0}' "$RUN_DIR/fork-dynamic-defined-symbols.txt")"
OFFICIAL_DYNAMIC_EXPORTS="$(awk 'NF {count++} END {print count+0}' "$RUN_DIR/official-dynamic-defined-symbols.txt")"

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
  echo "diagnostic_flags=$DIAGNOSTIC_FLAGS"
  echo "codegen_flags=$CODEGEN_FLAGS"
  echo "link_flags=$LINK_FLAGS"
  echo "strict_warning_gate=PASS"
  echo "fork_archive_defined_global_symbols=$FORK_GLOBAL_SYMBOLS"
  echo "official_archive_defined_global_symbols=$OFFICIAL_GLOBAL_SYMBOLS"
  echo "fork_dynamic_defined_symbols=$FORK_DYNAMIC_EXPORTS"
  echo "official_dynamic_defined_symbols=$OFFICIAL_DYNAMIC_EXPORTS"
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
  echo "comparison=fork_vs_official_same_device_same_compiler_same_strict_contract"
  echo "strict_warning_gate=PASS"
  echo "linker_gc_sections=ENABLED"
  echo "linker_build_id=NONE"
  echo "linker_exclude_libs=ALL"
  echo "official_median_MiB_s=$OFFICIAL_MEDIAN"
  echo "fork_median_MiB_s=$FORK_MEDIAN"
  echo "fork_over_official_ratio=${RATIO}x"
  echo "fork_gain_percent=${GAIN}%"
  echo "fork_archive_defined_global_symbols=$FORK_GLOBAL_SYMBOLS"
  echo "official_archive_defined_global_symbols=$OFFICIAL_GLOBAL_SYMBOLS"
  echo "fork_dynamic_defined_symbols=$FORK_DYNAMIC_EXPORTS"
  echo "official_dynamic_defined_symbols=$OFFICIAL_DYNAMIC_EXPORTS"
  echo "digest_equivalence=PASS"
  echo "source_core_modified_by_runner=NO"
  echo "benchmark_loop_scope=HARNESS_ONLY"
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
  "$RUN_DIR"/*.log \
  "$RUN_DIR"/*symbols*.txt \
  "$RUN_DIR"/*sections.txt \
  "$RUN_DIR"/*size.txt \
  "$RUN_DIR"/*disassembly.txt \
  "$FORK_BUILD/compile_commands.json" \
  "$OFFICIAL_BUILD/compile_commands.json" \
  "$FORK_BUILD/libblake3.a" \
  "$OFFICIAL_BUILD/libblake3.a" \
  "$FORK_BUILD/bench-rmr" \
  "$OFFICIAL_BUILD/bench-rmr" \
  > "$RUN_DIR/SHA256SUMS"

printf '\nreceipt=%s\n' "$RECEIPT"
printf 'hashes=%s\n' "$RUN_DIR/SHA256SUMS"
