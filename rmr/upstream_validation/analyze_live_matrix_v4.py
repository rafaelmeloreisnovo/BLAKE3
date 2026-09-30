#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
"""Paired analysis for RMR upstream live compile matrix V4."""
from __future__ import annotations
import argparse, csv, json, math, random, statistics
from collections import defaultdict
from pathlib import Path

def mean(xs): return statistics.fmean(xs)
def cv(xs):
    if len(xs) < 2 or mean(xs) == 0:
        return 0.0
    return statistics.stdev(xs) / mean(xs) * 100.0

def percentile(xs, p):
    xs=sorted(xs)
    if len(xs)==1: return xs[0]
    pos=(len(xs)-1)*p
    lo=math.floor(pos); hi=math.ceil(pos)
    if lo==hi: return xs[lo]
    f=pos-lo
    return xs[lo]*(1-f)+xs[hi]*f

def bootstrap_ratio(pairs, seed, n=10000):
    rng=random.Random(seed)
    out=[]
    for _ in range(n):
        draw=[pairs[rng.randrange(len(pairs))] for _ in range(len(pairs))]
        out.append(statistics.median(f/o for o,f in draw))
    return percentile(out,.025), percentile(out,.975)

def classify(lo, hi):
    if lo > 1.0: return "OBSERVED_GAIN_ON_THIS_RUNNER"
    if hi < 1.0: return "OBSERVED_LOSS_ON_THIS_RUNNER"
    return "PARITY_OR_NOISE_NOT_SEPARATED"

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--csv", required=True)
    ap.add_argument("--json", required=True)
    ap.add_argument("--md", required=True)
    a=ap.parse_args()
    rows=list(csv.DictReader(Path(a.csv).open(encoding="utf-8")))
    expected={"config_id","compiler","simd","cap","opt","lto","tbb","size_bytes","round","side","iterations","seconds","ns_per_op","mib_s","digest","lib_size_bytes","lib_sha256","config_sha256"}
    if not rows:
        raise SystemExit("results_csv_empty")
    for i,r in enumerate(rows, start=2):
        if set(r) != expected or None in r or any(r[k] in (None,"") for k in ("config_id","size_bytes","round","side","mib_s","digest","config_sha256")):
            raise SystemExit(f"results_csv_shape=FAIL line={i}")
    g=defaultdict(lambda: defaultdict(dict)); meta={}; digest=defaultdict(set); libmeta=defaultdict(dict)
    for r in rows:
        key=(r["config_id"], int(r["size_bytes"])); rd=int(r["round"]); side=r["side"]
        g[key][rd][side]=float(r["mib_s"]); digest[key].add(r["digest"])
        meta[r["config_id"]]={k:r[k] for k in ("compiler","simd","cap","opt","lto","tbb","config_sha256")}
        libmeta[r["config_id"]][side]={"size_bytes":int(r["lib_size_bytes"]),"sha256":r["lib_sha256"]}
    details=[]; by_config=defaultdict(list)
    for (config,size), rounds in sorted(g.items()):
        pairs=[]
        for rd in sorted(rounds):
            p=rounds[rd]
            if set(p) != {"official","fork"}: raise SystemExit(f"incomplete_pair config={config} size={size} round={rd}")
            pairs.append((p["official"],p["fork"]))
        if len(digest[(config,size)]) != 1: raise SystemExit(f"digest_mismatch config={config} size={size}")
        official=[x for x,_ in pairs]; fork=[x for _,x in pairs]; ratios=[f/o for o,f in pairs]
        seed=int(meta[config]["config_sha256"][:8],16) ^ size; lo,hi=bootstrap_ratio(pairs,seed)
        row={"config_id":config,"size_bytes":size,"rounds":len(pairs),**meta[config],"official_median_mib_s":statistics.median(official),"fork_median_mib_s":statistics.median(fork),"paired_ratio_median":statistics.median(ratios),"paired_delta_percent":(statistics.median(ratios)-1)*100,"official_cv_percent":cv(official),"fork_cv_percent":cv(fork),"bootstrap_95_low":lo,"bootstrap_95_high":hi,"classification":classify(lo,hi),"digest_equivalence":"PASS"}
        details.append(row); by_config[config].extend(ratios)
    configs=[]
    for config,ratios in sorted(by_config.items()):
        r=statistics.median(ratios); lm=libmeta[config]
        configs.append({"config_id":config,**meta[config],"paired_ratio_median_all_sizes":r,"paired_delta_percent_all_sizes":(r-1)*100,"official_lib_size_bytes":lm["official"]["size_bytes"],"fork_lib_size_bytes":lm["fork"]["size_bytes"],"lib_size_delta_percent":(lm["fork"]["size_bytes"]/lm["official"]["size_bytes"]-1)*100,"official_lib_sha256":lm["official"]["sha256"],"fork_lib_sha256":lm["fork"]["sha256"]})
    out={"schema":"RMR-UPSTREAM-LIVE-COMPILE-MATRIX-V4","claim_allowed":False,"details":details,"configs":configs,"boundaries":["LIVE_OFFICIAL_HEAD_IS_FROZEN_AT_RUN_START","COMMON_FLAGS_DO_NOT_IMPLY_IDENTICAL_SOURCE","RUNNER_OBSERVATION!=PHYSICAL_DEVICE_CLAIM","MEDIAN_DELTA!=STATISTICAL_SEPARATION","ONE_BIT_SOURCE_CHANGE!=ONE_BIT_BINARY_CHANGE"]}
    Path(a.json).write_text(json.dumps(out,indent=2,sort_keys=True)+"\n",encoding="utf-8")
    lines=["# RMR upstream live compile matrix V4","","SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM.","","| config | median Δ fork | lib size Δ | compiler | SIMD | cap | opt | LTO | TBB |","|---|---:|---:|---|---|---|---|---|---|"]
    for r in configs:
        lines.append(f"| `{r['config_id']}` | {r['paired_delta_percent_all_sizes']:+.3f}% | {r['lib_size_delta_percent']:+.3f}% | {r['compiler']} | {r['simd']} | {r['cap']} | {r['opt']} | {r['lto']} | {r['tbb']} |")
    lines += ["", "## Per-size statistical classification", "", "| config | bytes | Δ fork | 95% paired bootstrap | state | CV off/fork |", "|---|---:|---:|---|---|---:|"]
    for r in details:
        lines.append(f"| `{r['config_id']}` | {r['size_bytes']} | {r['paired_delta_percent']:+.3f}% | [{r['bootstrap_95_low']:.6f}, {r['bootstrap_95_high']:.6f}] | {r['classification']} | {r['official_cv_percent']:.2f}% / {r['fork_cv_percent']:.2f}% |")
    lines += ["", "All performance classifications are runner-local observations; claim_allowed=false.", ""]
    Path(a.md).write_text("\n".join(lines),encoding="utf-8")
    print(f"RMR_UPSTREAM_LIVE_ANALYSIS=PASS configs={len(configs)} rows={len(details)}")

if __name__=="__main__": main()
