#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-6aab490a26124663329dfd3961b8469f8fdb158b}"
CC_BIN="${CC:-clang}"
WORK="${WORK:-$ROOT/.rmr-work/binary-audit}"
OUT="${OUT:-$ROOT/rmr/validation/output/binary-audit}"

rm -rf "$WORK" "$OUT"; mkdir -p "$WORK" "$OUT"
git clone --filter=blob:none --no-tags "$OFFICIAL_REPO" "$WORK/upstream" >/dev/null 2>&1
git -C "$WORK/upstream" fetch --depth 1 origin "$OFFICIAL_REF" >/dev/null 2>&1
git -C "$WORK/upstream" checkout --detach FETCH_HEAD >/dev/null 2>&1

build_one() {
  local name="$1"; local src="$2"; local build="$WORK/build-$name"
  cmake -S "$src" -B "$build" -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER="$CC_BIN" -DBUILD_SHARED_LIBS=OFF -DBLAKE3_USE_TBB=OFF \
    >"$OUT/$name-configure.log" 2>&1
  cmake --build "$build" --parallel 2 >"$OUT/$name-build.log" 2>&1
  local lib="$(find "$build" -type f -name 'libblake3.a' -print -quit)"
  "$CC_BIN" -O3 -std=c11 -I"$src" "$ROOT/rmr/benchmark_framework/core/blake3_size_bench.c" "$lib" -o "$OUT/$name-bench"
  cp "$lib" "$OUT/$name-libblake3.a"
  nm -g --defined-only "$lib" >"$OUT/$name-symbols.txt"
  size -A "$lib" >"$OUT/$name-size.txt" || true
  readelf -h "$OUT/$name-bench" >"$OUT/$name-elf-header.txt"
  readelf -l -W "$OUT/$name-bench" >"$OUT/$name-program-headers.txt"
  readelf -S -W "$OUT/$name-bench" >"$OUT/$name-sections.txt"
}

build_one upstream "$WORK/upstream/c"
build_one fork "$ROOT/c"

STRICT_BUILD="$WORK/build-fork-strict"
if cmake -S "$ROOT/c" -B "$STRICT_BUILD" -G Ninja -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_C_COMPILER="$CC_BIN" -DBUILD_SHARED_LIBS=OFF -DBLAKE3_USE_TBB=OFF \
   -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic -Wshadow -Wpointer-arith -Wcast-align -Wformat=2 -Wundef -Wwrite-strings -Werror" \
   >"$OUT/fork-strict-configure.log" 2>&1 &&
   cmake --build "$STRICT_BUILD" --parallel 2 >"$OUT/fork-strict-build.log" 2>&1; then
  echo "STRICT_WARNINGS=PASS" >"$OUT/warnings.state"
else
  echo "STRICT_WARNINGS=REVIEW_LEGACY_OR_TOOLCHAIN" >"$OUT/warnings.state"
fi

python3 - "$OUT" <<'PY'
import json,re,subprocess,sys
from pathlib import Path
root=Path(sys.argv[1])
required={"blake3_version","blake3_hasher_init","blake3_hasher_init_keyed","blake3_hasher_init_derive_key","blake3_hasher_update","blake3_hasher_finalize","blake3_hasher_finalize_seek","blake3_hasher_reset"}
def symbols(name):
    out=set()
    for line in (root/f"{name}-symbols.txt").read_text().splitlines():
        parts=line.split()
        if parts: out.add(parts[-1])
    return out
def stack_exec(name):
    for line in (root/f"{name}-program-headers.txt").read_text().splitlines():
        if "GNU_STACK" in line:
            toks=line.split()
            flags=[x for x in toks if x and set(x)<=set("RWE") and any(c in x for c in "RWE")]
            return any("E" in x for x in flags)
    return None
u=symbols("upstream"); f=symbols("fork")
data={
 "schema":"RMR-BINARY-SURFACE-AUDIT-V1","claim_allowed":False,
 "required_api_upstream":sorted(required-u),"required_api_fork":sorted(required-f),
 "fork_extra_global_symbols":sorted(f-u),"fork_missing_vs_upstream":sorted(u-f),
 "upstream_archive_bytes":(root/"upstream-libblake3.a").stat().st_size,
 "fork_archive_bytes":(root/"fork-libblake3.a").stat().st_size,
 "upstream_bench_bytes":(root/"upstream-bench").stat().st_size,
 "fork_bench_bytes":(root/"fork-bench").stat().st_size,
 "upstream_gnu_stack_executable":stack_exec("upstream"),
 "fork_gnu_stack_executable":stack_exec("fork"),
 "strict_warning_state":(root/"warnings.state").read_text().strip().split("=",1)[1],
}
data["public_api_contract"]="PASS" if not data["required_api_upstream"] and not data["required_api_fork"] else "FAIL"
data["fork_gnu_stack_gate"]="PASS" if data["fork_gnu_stack_executable"] is False else "FAIL_OR_UNKNOWN"
(root/"summary.json").write_text(json.dumps(data,indent=2,sort_keys=True)+"\n")
print(json.dumps(data,indent=2))
if data["public_api_contract"]!="PASS" or data["fork_gnu_stack_executable"] is True: raise SystemExit(1)
PY

sha256sum "$OUT"/*-libblake3.a "$OUT"/*-bench >"$OUT/SHA256SUMS"
echo "RMR_BINARY_SURFACE_AUDIT=PASS"
