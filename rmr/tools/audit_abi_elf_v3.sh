#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/upstream_validation/common.sh"
for t in git cmake "${CC:-clang}" nm readelf size diff comm sha256sum; do rmr_need "$t"; done

WORK="${WORK_ROOT:-$ROOT/.rmr-work/upstream-v3-abi}"
OUT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/upstream-v3-abi}"
OFFICIAL_ROOT="$WORK/official"
rm -rf "$OUT" "$WORK/build-"*
mkdir -p "$WORK" "$OUT"
OFFICIAL_COMMIT="$(rmr_checkout_official "$OFFICIAL_ROOT")"
rmr_write_common_environment "$OUT/environment.txt" "$OFFICIAL_COMMIT"
mapfile -t GEN < <(rmr_generator_args)

audit_side() {
  local side="$1" src="$2" build="$WORK/build-$1"
  cmake -S "$src/c" -B "$build" "${GEN[@]}"     -DCMAKE_BUILD_TYPE=Release     -DCMAKE_C_COMPILER="${CC:-clang}"     -DCMAKE_CXX_COMPILER="${CXX:-clang++}"     -DBLAKE3_EXAMPLES=ON -DBLAKE3_USE_TBB=OFF     >"$OUT/$side-configure.log" 2>&1
  cmake --build "$build" --parallel 2 >"$OUT/$side-build.log" 2>&1

  local lib example
  lib="$(rmr_find_static_lib "$build")"
  example="$(find "$build" -type f -name 'blake3-example' -perm -111 -print -quit)"
  [ -n "$lib" ] && [ -n "$example" ]

  "${CC:-clang}" -std=c11 -O2 -I"$src/c"     "$ROOT/upstream_validation/blake3_abi_probe.c"     -o "$build/abi-probe"
  "$build/abi-probe" >"$OUT/$side-abi.txt"

  nm -g --defined-only "$lib" | awk 'NF {print $NF}' | sort -u >"$OUT/$side-symbols.txt"
  nm -u "$lib" | awk 'NF {print $NF}' | sort -u >"$OUT/$side-undefined.txt"
  readelf -Ws "$example" >"$OUT/$side-readelf-symbols.txt"
  readelf -W -S "$example" >"$OUT/$side-sections.txt"
  readelf -W -l "$example" >"$OUT/$side-program-headers.txt"
  size -A "$example" >"$OUT/$side-size.txt"
  stat -c '%s' "$lib" >"$OUT/$side-lib-bytes.txt"
  stat -c '%s' "$example" >"$OUT/$side-example-bytes.txt"

  if awk '/GNU_STACK/ { if ($0 ~ /RWE/) bad=1; found=1 } END { exit(!found || bad) }'       "$OUT/$side-program-headers.txt"; then
    echo "gnu_stack=PASS_NON_EXECUTABLE" >"$OUT/$side-stack.txt"
  else
    echo "gnu_stack=FAIL_OR_MISSING" >"$OUT/$side-stack.txt"
    return 1
  fi
}

audit_side official "$OFFICIAL_ROOT"
audit_side fork "$RMR_REPO_ROOT"

diff -u "$OUT/official-abi.txt" "$OUT/fork-abi.txt" >"$OUT/abi.diff" || true
comm -23 "$OUT/official-symbols.txt" "$OUT/fork-symbols.txt" >"$OUT/symbols-only-official.txt"
comm -13 "$OUT/official-symbols.txt" "$OUT/fork-symbols.txt" >"$OUT/symbols-only-fork.txt"

python3 - "$OUT" <<'PY'
import json, pathlib, sys
p=pathlib.Path(sys.argv[1])
def lines(name):
    return [x for x in (p/name).read_text().splitlines() if x.strip()]
abi_equal=(p/"abi.diff").read_text()=="" if (p/"abi.diff").exists() else True
payload={
 "schema":"RMR-BLAKE3-ABI-ELF-V3",
 "claim_allowed":False,
 "public_abi_probe_equal":abi_equal,
 "symbols_only_official":lines("symbols-only-official.txt"),
 "symbols_only_fork":lines("symbols-only-fork.txt"),
 "official_static_lib_bytes":int((p/"official-lib-bytes.txt").read_text()),
 "fork_static_lib_bytes":int((p/"fork-lib-bytes.txt").read_text()),
 "official_example_bytes":int((p/"official-example-bytes.txt").read_text()),
 "fork_example_bytes":int((p/"fork-example-bytes.txt").read_text()),
 "gnu_stack":{"official":"PASS_NON_EXECUTABLE","fork":"PASS_NON_EXECUTABLE"}
}
(p/"summary.json").write_text(json.dumps(payload,indent=2,sort_keys=True)+"\n")
print("RMR_ABI_ELF_V3=PASS")
print("public_abi_probe_equal="+str(abi_equal).lower())
print("symbols_only_official="+str(len(payload["symbols_only_official"])))
print("symbols_only_fork="+str(len(payload["symbols_only_fork"])))
PY

sha256sum "$OUT"/*.txt "$OUT"/*.json "$OUT"/*.diff >"$OUT/SHA256SUMS.txt"
