#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$ROOT/upstream_validation/common.sh"
for t in git cargo rustc python3 dd sha256sum stat; do rmr_need "$t"; done

WORK="${WORK_ROOT:-$ROOT/.rmr-work/upstream-v3-rust}"
OUT="${RESULT_ROOT:-$ROOT/rmr/benchmark_framework/output/upstream-v3-rust}"
OFFICIAL_ROOT="$WORK/official"
FORK_ROOT="$WORK/fork"
ROUNDS="${ROUNDS:-5}"
TARGET_MIB="${TARGET_MIB:-64}"

rm -rf "$OUT" "$WORK/target-"* "$FORK_ROOT"
mkdir -p "$WORK" "$OUT"
OFFICIAL_COMMIT="$(rmr_checkout_official "$OFFICIAL_ROOT")"
git clone --local "$RMR_REPO_ROOT" "$FORK_ROOT" >/dev/null 2>&1
git -C "$FORK_ROOT" checkout --detach "$(git -C "$RMR_REPO_ROOT" rev-parse HEAD)" >/dev/null 2>&1

rmr_write_common_environment "$OUT/environment.txt" "$OFFICIAL_COMMIT"
{
  echo "rustc=$(rustc --version)"
  echo "cargo=$(cargo --version)"
} >>"$OUT/environment.txt"

run_logged() {
  local stage="$1" log="$2"
  shift 2
  if "$@" >"$log" 2>&1; then
    echo "RMR_RUST_STAGE=$stage PASS"
  else
    rc=$?
    echo "RMR_RUST_STAGE=$stage FAIL rc=$rc" >&2
    echo "----- $log -----" >&2
    tail -n 240 "$log" >&2 || true
    exit "$rc"
  fi
}

run_logged official-blake3-test "$OUT/official-blake3-test.log"   cargo test --manifest-path "$OFFICIAL_ROOT/Cargo.toml" -p blake3 --lib
run_logged fork-blake3-test "$OUT/fork-blake3-test.log"   cargo test --manifest-path "$FORK_ROOT/Cargo.toml" -p blake3 --lib
run_logged official-b3sum-test "$OUT/official-b3sum-test.log"   cargo test --manifest-path "$OFFICIAL_ROOT/Cargo.toml" -p b3sum
run_logged fork-b3sum-test "$OUT/fork-b3sum-test.log"   cargo test --manifest-path "$FORK_ROOT/Cargo.toml" -p b3sum

mkdir -p "$OFFICIAL_ROOT/examples" "$FORK_ROOT/examples"
cp "$ROOT/upstream_validation/rust_hash_bench.rs" "$OFFICIAL_ROOT/examples/rmr_v3_hash_bench.rs"
cp "$ROOT/upstream_validation/rust_hash_bench.rs" "$FORK_ROOT/examples/rmr_v3_hash_bench.rs"

run_logged official-rust-harness-build "$OUT/official-rust-harness-build.log" env CARGO_TARGET_DIR="$WORK/target-rust-official" cargo build --manifest-path "$OFFICIAL_ROOT/Cargo.toml" --release --example rmr_v3_hash_bench

run_logged fork-rust-harness-build "$OUT/fork-rust-harness-build.log" env CARGO_TARGET_DIR="$WORK/target-rust-fork" cargo build --manifest-path "$FORK_ROOT/Cargo.toml" --release --example rmr_v3_hash_bench

run_logged fork-rust-harness-ablated-build "$OUT/fork-rust-harness-ablated-build.log" env CARGO_TARGET_DIR="$WORK/target-rust-fork-ablated" CARGO_PROFILE_RELEASE_LTO=false CARGO_PROFILE_RELEASE_CODEGEN_UNITS=16 cargo build --manifest-path "$FORK_ROOT/Cargo.toml" --release --example rmr_v3_hash_bench

RUST_OFFICIAL="$WORK/target-rust-official/release/examples/rmr_v3_hash_bench"
RUST_FORK="$WORK/target-rust-fork/release/examples/rmr_v3_hash_bench"
RUST_ABLATED="$WORK/target-rust-fork-ablated/release/examples/rmr_v3_hash_bench"

printf 'variant,size_bytes,round,mib_s,digest\n' >"$OUT/rust-library.csv"
for size in 64 1024 65536 1048576; do
  iters="$(python3 - "$size" "$TARGET_MIB" <<'PY'
import math,sys
print(max(1,math.ceil(int(sys.argv[2])*1024*1024/int(sys.argv[1]))))
PY
)"
  for spec in "official:$RUST_OFFICIAL" "fork:$RUST_FORK" "fork_ablated:$RUST_ABLATED"; do
    name="${spec%%:*}"; bin="${spec#*:}"
    "$bin" "$size" "$iters" >/dev/null
    for ((r=1;r<=ROUNDS;r++)); do
      line="$("$bin" "$size" "$iters")"
      printf '%s\n' "$line" | awk -F, -v v="$name" -v r="$r"         '$1=="RUST_RESULT"{printf "%s,%s,%s,%s,%s\n",v,$2,r,$4,$6}'         >>"$OUT/rust-library.csv"
    done
  done
done

python3 - "$OUT/rust-library.csv" "$OUT/rust-library-summary.json" "$RUST_OFFICIAL" "$RUST_FORK" "$RUST_ABLATED" <<'PY'
import csv,json,os,statistics,sys
from collections import defaultdict
rows=list(csv.DictReader(open(sys.argv[1],encoding="utf-8")))
g=defaultdict(list); dig=defaultdict(set)
for r in rows:
    k=(r["variant"],int(r["size_bytes"]))
    g[k].append(float(r["mib_s"])); dig[int(r["size_bytes"])].add(r["digest"])
assert all(len(x)==1 for x in dig.values())
out=[]
for (v,size),xs in sorted(g.items()):
    out.append({"variant":v,"size_bytes":size,"rounds":len(xs),
                "median_mib_s":statistics.median(xs),
                "mean_mib_s":statistics.fmean(xs),
                "cv_percent":statistics.stdev(xs)/statistics.fmean(xs)*100 if len(xs)>1 else 0.0})
payload={"schema":"RMR-RUST-LIBRARY-V3","claim_allowed":False,
         "digest_equivalence":"PASS","rows":out,
         "binary_sizes":{
           "official":os.path.getsize(sys.argv[3]),
           "fork":os.path.getsize(sys.argv[4]),
           "fork_ablated":os.path.getsize(sys.argv[5])
         }}
json.dump(payload,open(sys.argv[2],"w"),indent=2)
print("RMR_RUST_LIBRARY_V3=PASS")
for r in out: print(r)
PY

run_logged official-release-build "$OUT/official-release-build.log" env CARGO_TARGET_DIR="$WORK/target-official" cargo build --manifest-path "$OFFICIAL_ROOT/Cargo.toml" --release -p b3sum
run_logged fork-release-build "$OUT/fork-release-build.log" env CARGO_TARGET_DIR="$WORK/target-fork" cargo build --manifest-path "$FORK_ROOT/Cargo.toml" --release -p b3sum
run_logged fork-ablated-build "$OUT/fork-ablated-build.log" env CARGO_TARGET_DIR="$WORK/target-fork-ablated" CARGO_PROFILE_RELEASE_LTO=false CARGO_PROFILE_RELEASE_CODEGEN_UNITS=16 cargo build --manifest-path "$FORK_ROOT/Cargo.toml" --release -p b3sum

OFFICIAL_BIN="$WORK/target-official/release/b3sum"
FORK_BIN="$WORK/target-fork/release/b3sum"
ABLATE_BIN="$WORK/target-fork-ablated/release/b3sum"
for b in "$OFFICIAL_BIN" "$FORK_BIN" "$ABLATE_BIN"; do [ -x "$b" ]; done

dd if=/dev/zero of="$WORK/data64m.bin" bs=1M count=64 status=none
: >"$WORK/tiny.bin"

OFFICIAL_DIGEST="$("$OFFICIAL_BIN" --num-threads 1 "$WORK/data64m.bin" | awk '{print $1}')"
FORK_DIGEST="$("$FORK_BIN" --num-threads 1 "$WORK/data64m.bin" | awk '{print $1}')"
ABLATE_DIGEST="$("$ABLATE_BIN" --num-threads 1 "$WORK/data64m.bin" | awk '{print $1}')"
[ "$OFFICIAL_DIGEST" = "$FORK_DIGEST" ] && [ "$OFFICIAL_DIGEST" = "$ABLATE_DIGEST" ]

TINY_DIGEST="$("$OFFICIAL_BIN" "$WORK/tiny.bin" | awk '{print $1}')"
python3 - "$WORK/checkfile.txt" "$TINY_DIGEST" "$WORK/tiny.bin" <<'PY'
import sys
path,digest,target=sys.argv[1:]
with open(path,"w",encoding="utf-8") as f:
    for _ in range(20000):
        f.write(f"{digest}  {target}\n")
PY

printf 'binary,workload,round,seconds\n' >"$OUT/timings.csv"

python3 - "$OUT/timings.csv" "$ROUNDS" "$OFFICIAL_BIN" "$FORK_BIN" "$ABLATE_BIN" "$WORK/data64m.bin" "$WORK/checkfile.txt" <<'PY'
import csv,subprocess,sys,time
csv_path,rounds,official,fork,ablated,data,check=sys.argv[1:]
rounds=int(rounds)
bins=[("official",official),("fork",fork),("fork_ablated",ablated)]
workloads=[
 ("hash_1thread",lambda b:[b,"--num-threads","1",data]),
 ("hash_4thread",lambda b:[b,"--num-threads","4",data]),
 ("checkfile_20k",lambda b:[b,"--check","--quiet",check]),
]
with open(csv_path,"a",newline="",encoding="utf-8") as f:
    w=csv.writer(f)
    for workload,cmdfn in workloads:
        for r in range(1,rounds+1):
            order=bins[r%len(bins):]+bins[:r%len(bins)]
            for name,b in order:
                t0=time.perf_counter()
                cp=subprocess.run(cmdfn(b),stdout=subprocess.DEVNULL,stderr=subprocess.PIPE)
                dt=time.perf_counter()-t0
                if cp.returncode:
                    raise SystemExit(f"{name}/{workload} failed: {cp.stderr.decode(errors='replace')}")
                w.writerow([name,workload,r,f"{dt:.9f}"])
PY

{
  echo "official_b3sum_bytes=$(stat -c '%s' "$OFFICIAL_BIN")"
  echo "fork_b3sum_bytes=$(stat -c '%s' "$FORK_BIN")"
  echo "fork_ablated_b3sum_bytes=$(stat -c '%s' "$ABLATE_BIN")"
  echo "digest_equivalence=PASS"
} >"$OUT/binary-sizes.txt"

python3 "$ROOT/upstream_validation/analyze_rust_cli_v3.py"   "$OUT/timings.csv" "$OUT/summary.json" | tee "$OUT/analysis.txt"

{
  echo "RMR_RUST_CLI_V3=PASS"
  echo "cargo_test_blake3=PASS_BOTH"
  echo "cargo_test_b3sum=PASS_BOTH"
  echo "rust_library_common_harness=PASS"
  echo "release_build=PASS_OFFICIAL_FORK_ABLATED"
  echo "digest_equivalence=PASS"
  echo "claim_allowed=false"
} >"$OUT/receipt.txt"
sha256sum "$OUT"/*.txt "$OUT"/*.csv "$OUT"/*.json >"$OUT/SHA256SUMS.txt"
