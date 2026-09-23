#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-6aab490a26124663329dfd3961b8469f8fdb158b}"
WORK="${WORK:-/tmp/rmr-rust-cli}"
OUT="${OUT:-$ROOT/rmr/validation/output/rust-cli}"
ROUNDS="${ROUNDS:-5}"
TARGET_MIB="${TARGET_MIB:-64}"

rm -rf "$WORK" "$OUT"
mkdir -p "$WORK" "$OUT"
git clone --filter=blob:none --no-tags "$OFFICIAL_REPO" "$WORK/upstream" >/dev/null 2>&1
git -C "$WORK/upstream" fetch --depth 1 origin "$OFFICIAL_REF" >/dev/null 2>&1
git -C "$WORK/upstream" checkout --detach FETCH_HEAD >/dev/null 2>&1

mkdir -p "$WORK/upstream/examples" "$WORK/fork/examples"
cp -a "$ROOT/." "$WORK/fork/"
cp "$ROOT/rmr/validation/rust_hash_bench.rs" "$WORK/upstream/examples/rmr_validation_bench.rs"
cp "$ROOT/rmr/validation/rust_hash_bench.rs" "$WORK/fork/examples/rmr_validation_bench.rs"

echo "[rust] upstream tests"
(cd "$WORK/upstream" && cargo test --release) >"$OUT/upstream-rust-test.txt" 2>&1
echo "[rust] fork tests"
(cd "$WORK/fork" && cargo test --release) >"$OUT/fork-rust-test.txt" 2>&1

echo "[rust] build common harness variants"
CARGO_TARGET_DIR="$WORK/target-upstream" cargo build --release --manifest-path "$WORK/upstream/Cargo.toml" --example rmr_validation_bench >"$OUT/upstream-rust-build.txt" 2>&1
CARGO_TARGET_DIR="$WORK/target-fork-tuned" cargo build --release --manifest-path "$WORK/fork/Cargo.toml" --example rmr_validation_bench >"$OUT/fork-tuned-rust-build.txt" 2>&1
CARGO_TARGET_DIR="$WORK/target-fork-untuned" CARGO_PROFILE_RELEASE_LTO=false CARGO_PROFILE_RELEASE_CODEGEN_UNITS=16 cargo build --release --manifest-path "$WORK/fork/Cargo.toml" --example rmr_validation_bench >"$OUT/fork-untuned-rust-build.txt" 2>&1

UP="$WORK/target-upstream/release/examples/rmr_validation_bench"
FT="$WORK/target-fork-tuned/release/examples/rmr_validation_bench"
FU="$WORK/target-fork-untuned/release/examples/rmr_validation_bench"

printf 'variant,size_bytes,round,iterations,ns_per_op,mib_s,digest\n' >"$OUT/rust-results.csv"
for size in 64 1024 65536 1048576; do
  iters="$(python3 - "$size" "$TARGET_MIB" <<'PY'
import math,sys
print(max(1,math.ceil(int(sys.argv[2])*1024*1024/int(sys.argv[1]))))
PY
)"
  for spec in "upstream:$UP" "fork_tuned:$FT" "fork_untuned:$FU"; do
    name="${spec%%:*}"; bin="${spec#*:}"
    "$bin" "$size" "$iters" >/dev/null
    for ((round=1; round<=ROUNDS; round++)); do
      line="$("$bin" "$size" "$iters")"
      printf '%s\n' "$line" | awk -F, -v v="$name" -v r="$round" '$1=="RUST_RESULT"{printf "%s,%s,%s,%s,%s,%s,%s\n",v,$2,r,$3,$4,$5,$7}' >>"$OUT/rust-results.csv"
    done
  done
done

echo "[b3sum] tests and builds"
(cd "$WORK/upstream/b3sum" && cargo test --release) >"$OUT/upstream-b3sum-test.txt" 2>&1
(cd "$WORK/fork/b3sum" && cargo test --release) >"$OUT/fork-b3sum-test.txt" 2>&1
CARGO_TARGET_DIR="$WORK/target-b3sum-upstream" cargo build --release --manifest-path "$WORK/upstream/b3sum/Cargo.toml" >"$OUT/upstream-b3sum-build.txt" 2>&1
CARGO_TARGET_DIR="$WORK/target-b3sum-fork" cargo build --release --manifest-path "$WORK/fork/b3sum/Cargo.toml" >"$OUT/fork-b3sum-build.txt" 2>&1
BU="$WORK/target-b3sum-upstream/release/b3sum"
BF="$WORK/target-b3sum-fork/release/b3sum"

mkdir -p "$WORK/fixtures"
python3 - "$WORK/fixtures" <<'PY'
from pathlib import Path
import sys
root=Path(sys.argv[1])
def write(path,n):
    chunk=bytes((i*131+17)&255 for i in range(65536))
    with path.open("wb") as f:
        left=n
        while left:
            x=chunk[:min(left,len(chunk))]; f.write(x); left-=len(x)
write(root/"one-mib.bin",1<<20)
write(root/"sixtyfour-mib.bin",64<<20)
(root/"empty.dat").write_bytes(b"")
PY
EMPTY_HASH="$("$BF" "$WORK/fixtures/empty.dat" | awk '{print $1}')"
python3 - "$WORK/fixtures/check.txt" "$EMPTY_HASH" "$WORK/fixtures/empty.dat" <<'PY'
from pathlib import Path
import sys
p=Path(sys.argv[1]); h=sys.argv[2]; f=sys.argv[3]
p.write_text("".join(f"{h}  {f}\n" for _ in range(20000)),encoding="utf-8")
PY

python3 - "$BU" "$BF" "$WORK/fixtures" "$OUT/b3sum-summary.json" <<'PY'
import json,statistics,subprocess,sys,time,os
up,fork,fixtures,out=sys.argv[1:]
workloads=[
 ("hash_1m_single",[os.path.join(fixtures,"one-mib.bin")]),
 ("hash_64m_single",[os.path.join(fixtures,"sixtyfour-mib.bin")]),
 ("hash_64m_no_mmap",["--no-mmap","--num-threads","1",os.path.join(fixtures,"sixtyfour-mib.bin")]),
 ("check_20k_empty",["--check","--quiet",os.path.join(fixtures,"check.txt")]),
]
rows=[]
for name,args in workloads:
    outputs={}
    for label,bin in [("upstream",up),("fork",fork)]:
        vals=[]
        for _ in range(2): subprocess.run([bin]+args,stdout=subprocess.PIPE,stderr=subprocess.PIPE,check=True)
        for _ in range(5):
            t=time.perf_counter_ns(); cp=subprocess.run([bin]+args,stdout=subprocess.PIPE,stderr=subprocess.PIPE,check=True); vals.append(time.perf_counter_ns()-t)
            outputs[label]=cp.stdout
        rows.append({"workload":name,"variant":label,"median_ns":statistics.median(vals),"mean_ns":statistics.fmean(vals),"cv_percent":statistics.stdev(vals)/statistics.fmean(vals)*100 if len(vals)>1 else 0.0})
    if name.startswith("hash_") and outputs["upstream"] != outputs["fork"]: raise SystemExit(f"output mismatch {name}")
for name,_ in workloads:
    u=next(r for r in rows if r["workload"]==name and r["variant"]=="upstream")
    f=next(r for r in rows if r["workload"]==name and r["variant"]=="fork")
    f["ratio_time_fork_over_upstream"]=f["median_ns"]/u["median_ns"]
    f["speedup_upstream_over_fork"]=u["median_ns"]/f["median_ns"]
data={"schema":"RMR-B3SUM-COMPARE-V1","output_equivalence":"PASS","claim_allowed":False,"rows":rows}
json.dump(data,open(out,"w"),indent=2)
print("RMR_B3SUM_COMPARE=PASS")
for r in rows: print(r)
PY

python3 - "$OUT/rust-results.csv" "$OUT/rust-summary.json" "$UP" "$FT" "$FU" "$BU" "$BF" <<'PY'
import csv,json,os,statistics,sys
inp,out,*bins=sys.argv[1:]
from collections import defaultdict
rows=list(csv.DictReader(open(inp,encoding="utf-8"))); g=defaultdict(list); dig=defaultdict(set)
for r in rows:
    key=(r["variant"],int(r["size_bytes"])); g[key].append(float(r["mib_s"])); dig[int(r["size_bytes"])].add(r["digest"])
assert all(len(v)==1 for v in dig.values())
s=[]
for (v,size),xs in sorted(g.items()): s.append({"variant":v,"size_bytes":size,"median_mib_s":statistics.median(xs),"mean_mib_s":statistics.fmean(xs),"cv_percent":statistics.stdev(xs)/statistics.fmean(xs)*100 if len(xs)>1 else 0.0})
data={"schema":"RMR-RUST-LTO-COMPARE-V1","digest_equivalence":"PASS","claim_allowed":False,"summary":s,"binary_sizes":{os.path.basename(b)+":"+str(i):os.path.getsize(b) for i,b in enumerate(bins)}}
json.dump(data,open(out,"w"),indent=2)
print("RMR_RUST_LTO_COMPARE=PASS")
for r in s: print(r)
PY

echo "RMR_RUST_TESTS=PASS"
echo "RMR_B3SUM_TESTS=PASS"
