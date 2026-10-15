#!/usr/bin/env bash
# SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
# Pure BLAKE3: native CMake and Rust source x build policy, no JNI/RMR.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_SHA=f55849f89cd85083c1c9daa2c5de20766f309c00
RUST_TOOLCHAIN="${RUST_TOOLCHAIN:-1.99.0}"
ROUNDS="${ROUNDS:-7}"
TARGET_MIB="${TARGET_MIB:-32}"
SIZES="${SIZES:-64 1024 65536 1048576}"
for n in "$ROUNDS" "$TARGET_MIB"; do
  [[ "$n" =~ ^[0-9]+$ ]] || { echo invalid_numeric_parameter >&2; exit 2; }
done
(( ROUNDS >= 3 && TARGET_MIB >= 1 )) || exit 2
for t in git cmake ninja cc python3 sha256sum stat cargo rustc rustup awk cmp find; do
  command -v "$t" >/dev/null || { echo "missing_tool=$t" >&2; exit 127; }
done
[[ -z "${CARGO_PROFILE_RELEASE_LTO:-}" && -z "${CARGO_PROFILE_RELEASE_CODEGEN_UNITS:-}" && -z "${RUSTFLAGS:-}" ]] || {
  echo external_rust_profile_override=FAIL >&2; exit 3;
}
FORK_SHA="$(git -C "$ROOT" rev-parse HEAD)"
[[ -z "${EXPECTED_FORK_SHA:-}" || "$EXPECTED_FORK_SHA" == "$FORK_SHA" ]] || { echo fork_sha_mismatch >&2; exit 3; }
OUT="$ROOT/rmr/benchmark_framework/output/pure-build-delta-v1"
WORK="$ROOT/.rmr-work/pure-build-delta-v1"
OFFICIAL="$WORK/official"
rm -rf "$OUT" "$WORK"
mkdir -p "$OUT" "$OFFICIAL"
git -C "$OFFICIAL" init -q
git -C "$OFFICIAL" remote add origin https://github.com/BLAKE3-team/BLAKE3.git
git -C "$OFFICIAL" fetch -q --no-tags --depth=1 origin "$OFFICIAL_SHA"
git -C "$OFFICIAL" checkout -q --detach FETCH_HEAD
[[ "$(git -C "$OFFICIAL" rev-parse HEAD)" == "$OFFICIAL_SHA" ]] || exit 4
FORK_TREE="$(git -C "$ROOT" rev-parse HEAD^{tree})"
OFFICIAL_TREE="$(git -C "$OFFICIAL" rev-parse HEAD^{tree})"
HARNESS_SHA="$(sha256sum "$ROOT/rmr/benchmark_framework/core/pure_cargo_bench_v1.rs" | awk '{print $1}')"
for src in "$OFFICIAL" "$ROOT"; do
  mkdir -p "$src/examples"
  [[ ! -e "$src/examples/rmr_pure_build_delta_v1.rs" ]] || { echo example_name_collision >&2; exit 5; }
  cp "$ROOT/rmr/benchmark_framework/core/pure_cargo_bench_v1.rs" "$src/examples/rmr_pure_build_delta_v1.rs"
done
for side in official fork; do
  if [[ "$side" == official ]]; then src="$OFFICIAL"; else src="$ROOT"; fi
  (cd "$src" && RUSTUP_TOOLCHAIN="$RUST_TOOLCHAIN" cargo generate-lockfile --offline) >"$OUT/${side}-lock.log" 2>&1 || {
    (cd "$src" && RUSTUP_TOOLCHAIN="$RUST_TOOLCHAIN" cargo generate-lockfile) >>"$OUT/${side}-lock.log" 2>&1
  }
  cp "$src/Cargo.lock" "$OUT/${side}-Cargo.lock"
done
cmp "$OUT/official-Cargo.lock" "$OUT/fork-Cargo.lock" || { echo dependency_lock_parity=FAIL >&2; exit 6; }
printf 'variant,size_bytes,round,iterations,nanoseconds,digest,checksum\n' >"$OUT/rust-results.csv"
printf 'variant,size_bytes,round,iterations,nanoseconds,digest,checksum\n' >"$OUT/c-results.csv"
printf 'variant,engine,source_sha,policy,binary_bytes,binary_sha256,compiler_commands_sha256\n' >"$OUT/binaries.csv"

for variant in official_rust_native official_rust_tuned fork_rust_stock fork_rust_native; do
  case "$variant" in
    official_*) src="$OFFICIAL"; source_sha="$OFFICIAL_SHA" ;;
    fork_*) src="$ROOT"; source_sha="$FORK_SHA" ;;
  esac
  args=()
  case "$variant" in
    official_rust_tuned) args=(--config profile.release.lto=true --config profile.release.codegen-units=1) ;;
    fork_rust_stock) args=(--config 'profile.release.lto="off"' --config profile.release.codegen-units=16) ;;
  esac
  target="$WORK/rust-target/$variant"
  RUSTUP_TOOLCHAIN="$RUST_TOOLCHAIN" CARGO_INCREMENTAL=0 cargo -vv "${args[@]}" build \
    --manifest-path "$src/Cargo.toml" --release --locked \
    --example rmr_pure_build_delta_v1 --target-dir "$target" \
    >"$OUT/$variant-build.log" 2>&1
  exe="$target/release/examples/rmr_pure_build_delta_v1"
  [[ -x "$exe" ]] || { echo "missing_binary=$variant" >&2; exit 7; }
  printf '%s,rust,%s,%s,%s,%s,%s\n' "$variant" "$source_sha" "${args[*]:-native}" \
    "$(stat -c%s "$exe")" "$(sha256sum "$exe" | awk '{print $1}')" \
    "$(sha256sum "$OUT/$variant-build.log" | awk '{print $1}')" >>"$OUT/binaries.csv"
  printf '%s|%s\n' "$variant" "$exe" >>"$OUT/rust-executables.txt"
done

for side in official fork; do
  if [[ "$side" == official ]]; then src="$OFFICIAL"; source_sha="$OFFICIAL_SHA"; else src="$ROOT"; source_sha="$FORK_SHA"; fi
  build="$WORK/c-build/$side"
  cmake -S "$src/c" -B "$build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_SHARED_LIBS=OFF -DBLAKE3_USE_TBB=OFF -DBLAKE3_FETCH_TBB=OFF \
    -DBLAKE3_EXAMPLES=OFF >"$OUT/${side}-c-configure.log" 2>&1
  cmake --build "$build" --parallel 2 >"$OUT/${side}-c-build.log" 2>&1
  lib="$(find "$build" -name libblake3.a -type f -print -quit)"
  [[ -n "$lib" ]] || { echo "c_lib_missing=$side" >&2; exit 8; }
  cp "$build/compile_commands.json" "$OUT/${side}-c-compile_commands.json"
  exe="$build/pure-c-bench"
  cc -O3 -std=c11 -I"$src/c" "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" "$lib" -o "$exe"
  printf '%s|%s\n' "${side}_c_native" "$exe" >>"$OUT/c-executables.txt"
  printf '%s,c,%s,native-cmake,%s,%s,%s\n' "${side}_c_native" "$source_sha" \
    "$(stat -c%s "$lib")" "$(sha256sum "$lib" | awk '{print $1}')" \
    "$(sha256sum "$build/compile_commands.json" | awk '{print $1}')" >>"$OUT/binaries.csv"
done
python3 "$ROOT/rmr/upstream_validation/analyze_pure_build_delta_v1.py" \
  --commands "$OUT/official-c-compile_commands.json" "$OUT/fork-c-compile_commands.json" \
  --official-root "$OFFICIAL" --fork-root "$ROOT" --work "$WORK" \
  --commands-out "$OUT/c-effective-commands.diff"

iterations_for() {
  python3 - "$1" "$TARGET_MIB" <<'PY'
import sys
size=int(sys.argv[1]); target=int(sys.argv[2])*1024*1024
print(max(1,(target+size-1)//size))
PY
}
get_exe() { awk -F'|' -v k="$1" '$1==k{print $2}' "$OUT/$2-executables.txt"; }
for size in $SIZES; do
  [[ "$size" =~ ^[0-9]+$ ]] && (( size > 0 && size <= 1048576 )) || { echo size_domain_invalid >&2; exit 9; }
  iters="$(iterations_for "$size")"
  warm=$(( iters / 8 )); (( warm >= 1 )) || warm=1
  for variant in official_rust_native official_rust_tuned fork_rust_stock fork_rust_native; do
    "$(get_exe "$variant" rust)" "$size" "$warm" >/dev/null
  done
  for variant in official_c_native fork_c_native; do
    "$(get_exe "$variant" c)" warmup "$size" "$warm" >/dev/null
  done
  for ((round=1; round<=ROUNDS; round++)); do
    rust_order=(official_rust_native fork_rust_native official_rust_tuned fork_rust_stock)
    c_order=(official_c_native fork_c_native)
    if (( round % 2 == 0 )); then
      rust_order=(fork_rust_stock official_rust_tuned fork_rust_native official_rust_native)
      c_order=(fork_c_native official_c_native)
    fi
    for variant in "${rust_order[@]}"; do
      line="$("$(get_exe "$variant" rust)" "$size" "$iters")"
      printf '%s,%s,%s,%s,%s\n' "$variant" "$size" "$round" "$iters" "$line" >>"$OUT/rust-results.csv"
    done
    for variant in "${c_order[@]}"; do
      line="$("$(get_exe "$variant" c)" "$variant" "$size" "$iters")"
      printf '%s\n' "$line" | awk -F, -v v="$variant" -v rd="$round" \
        '$1=="SIZE_RESULT" && NF==9 {printf "%s,%s,%s,%s,%.0f,%s,%s\n",v,$3,rd,$4,$5*1000000000,$9,$8; ok=1} END{if(!ok)exit 1}' \
        >>"$OUT/c-results.csv"
    done
  done
done
python3 "$ROOT/rmr/upstream_validation/analyze_pure_build_delta_v1.py" \
 --rust "$OUT/rust-results.csv" --c "$OUT/c-results.csv" \
 --json "$OUT/summary.json" --md "$OUT/summary.md"
{
  echo schema=RMR-PURE-BUILD-DELTA-V1
  echo "official_sha=$OFFICIAL_SHA"
  echo "fork_sha=$FORK_SHA"
  echo "official_tree=$OFFICIAL_TREE"
  echo "fork_tree=$FORK_TREE"
  echo "harness_sha256=$HARNESS_SHA"
  echo "rust_toolchain=$RUST_TOOLCHAIN"
  echo "rustc_version=$(RUSTUP_TOOLCHAIN="$RUST_TOOLCHAIN" rustc --version)"
  echo "cargo_version=$(RUSTUP_TOOLCHAIN="$RUST_TOOLCHAIN" cargo --version)"
  echo "c_compiler=$(cc --version | head -n1)"
  echo "cmake_version=$(cmake --version | head -n1)"
  echo cargo_lock_parity=PASS
  echo "sizes=$SIZES"
  echo "rounds=$ROUNDS"
  echo "target_mib=$TARGET_MIB"
  echo jni_rmr_provider_layers=ABSENT
  echo host_measurement_clock=PRESENT
  echo physical_arm32_arm64=NOT_RUN
  echo confidence_scope=RUNNER_LOCAL
  echo claim_allowed=false
} >"$OUT/receipt.txt"
(
 cd "$OUT"
 sha256sum binaries.csv c-results.csv rust-results.csv summary.json summary.md \
   official-Cargo.lock fork-Cargo.lock c-effective-commands.diff receipt.txt > SHA256SUMS.txt
)
echo RMR_PURE_BUILD_DELTA=PASS_RUNNER_LOCAL
cat "$OUT/summary.md"
