#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
"""Analyze CPU-pinned BLAKE3_LIKELY policy matrix V2."""
from __future__ import annotations
import argparse,csv,json,math,random,statistics
from collections import Counter,defaultdict
from pathlib import Path

VARIANTS=("official","fork","fork_no_likely")
COMPILERS=("gcc","clang")
CAPS=("sse2","sse41","avx2","auto")
COMPARISONS=(
    ("fork_over_official","fork","official"),
    ("no_likely_over_fork","fork_no_likely","fork"),
    ("no_likely_over_official","fork_no_likely","official"),
)

def mean(xs): return statistics.fmean(xs)
def cv(xs):
    m=mean(xs)
    return statistics.stdev(xs)/m*100.0 if len(xs)>1 and m else 0.0
def percentile(xs,p):
    xs=sorted(xs)
    if len(xs)==1: return xs[0]
    pos=(len(xs)-1)*p; lo=math.floor(pos); hi=math.ceil(pos)
    if lo==hi: return xs[lo]
    f=pos-lo; return xs[lo]*(1-f)+xs[hi]*f
def bootstrap(pairs,seed,n=10000):
    rng=random.Random(seed); vals=[]
    for _ in range(n):
        draw=[pairs[rng.randrange(len(pairs))] for _ in range(len(pairs))]
        vals.append(statistics.median(a/b for a,b in draw))
    return percentile(vals,.025),percentile(vals,.975)
def classify(lo,hi):
    if lo>1.0: return "OBSERVED_GAIN_ON_THIS_RUNNER"
    if hi<1.0: return "OBSERVED_LOSS_ON_THIS_RUNNER"
    return "PARITY_OR_NOISE_NOT_SEPARATED"

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--csv",required=True)
    ap.add_argument("--objects",required=True)
    ap.add_argument("--json",required=True)
    ap.add_argument("--md",required=True)
    a=ap.parse_args()
    rows=list(csv.DictReader(Path(a.csv).open(encoding="utf-8")))
    required={"compiler","cap","variant","size_bytes","round","position","iterations","seconds","ns_per_op","mib_s","digest"}
    if not rows: raise SystemExit("results=EMPTY")
    for i,r in enumerate(rows,start=2):
        if set(r)!=required or None in r or any(r[k] in (None,"") for k in required):
            raise SystemExit(f"results_shape=FAIL line={i}")
        if r["variant"] not in VARIANTS or r["compiler"] not in COMPILERS or r["cap"] not in CAPS:
            raise SystemExit(f"domain=FAIL line={i}")

    g=defaultdict(lambda:defaultdict(dict))
    positions=defaultdict(Counter)
    digests=defaultdict(set)
    raw_ratios=defaultdict(list)
    for r in rows:
        compiler=r["compiler"]; cap=r["cap"]; size=int(r["size_bytes"]); rd=int(r["round"]); v=r["variant"]
        key=(compiler,cap,size)
        g[key][rd][v]=float(r["mib_s"])
        positions[(compiler,cap,v)][int(r["position"])]+=1
        digests[key].add(r["digest"])
    for key,d in digests.items():
        if len(d)!=1: raise SystemExit(f"digest_equivalence=FAIL key={key}")
    for key,rounds in g.items():
        for rd,p in rounds.items():
            if set(p)!=set(VARIANTS): raise SystemExit(f"round_completeness=FAIL key={key} round={rd}")
    for key,c in positions.items():
        vals=list(c.values())
        if set(c)!=set(range(3)) or max(vals)!=min(vals):
            raise SystemExit(f"position_balance=FAIL key={key} counts={dict(c)}")

    details=[]
    for (compiler,cap,size),rounds in sorted(g.items()):
        for label,num,den in COMPARISONS:
            pairs=[(rounds[rd][num],rounds[rd][den]) for rd in sorted(rounds)]
            ratios=[x/y for x,y in pairs]
            raw_ratios[(compiler,cap,label)].extend(ratios)
            seed=(size ^ sum(map(ord,compiler+cap+label))) & 0xffffffff
            lo,hi=bootstrap(pairs,seed)
            numxs=[x for x,_ in pairs]; denxs=[y for _,y in pairs]
            details.append({
                "compiler":compiler,"cap":cap,"comparison":label,"size_bytes":size,"rounds":len(pairs),
                "numerator":num,"denominator":den,
                "numerator_median_mib_s":statistics.median(numxs),
                "denominator_median_mib_s":statistics.median(denxs),
                "paired_ratio_median":statistics.median(ratios),
                "paired_delta_percent":(statistics.median(ratios)-1.0)*100.0,
                "bootstrap_95_low":lo,"bootstrap_95_high":hi,
                "classification":classify(lo,hi),
                "numerator_cv_percent":cv(numxs),"denominator_cv_percent":cv(denxs),
            })

    aggregate=[]
    for (compiler,cap,label),ratios in sorted(raw_ratios.items()):
        med=statistics.median(ratios)
        aggregate.append({
            "compiler":compiler,"cap":cap,"comparison":label,
            "observations":len(ratios),
            "paired_ratio_median_all_sizes":med,
            "paired_delta_percent_all_sizes":(med-1.0)*100.0,
            "ratio_min":min(ratios),"ratio_max":max(ratios),
        })

    objrows=list(csv.DictReader(Path(a.objects).open(encoding="utf-8")))
    obj_required={"compiler","cap","variant","artifact","sha256","size_bytes","simd_degree"}
    om=defaultdict(dict)
    for i,r in enumerate(objrows,start=2):
        if set(r)!=obj_required or None in r: raise SystemExit(f"objects_shape=FAIL line={i}")
        key=(r["compiler"],r["cap"],r["artifact"])
        om[key][r["variant"]]={"sha256":r["sha256"],"size_bytes":int(r["size_bytes"]),"simd_degree":int(r["simd_degree"])}
    structural=[]
    for (compiler,cap,artifact),vs in sorted(om.items()):
        if not set(VARIANTS).issubset(vs): raise SystemExit(f"object_matrix=FAIL {compiler} {cap} {artifact}")
        structural.append({
            "compiler":compiler,"cap":cap,"artifact":artifact,
            "official_equal_fork":vs["official"]["sha256"]==vs["fork"]["sha256"],
            "no_likely_equal_official":vs["fork_no_likely"]["sha256"]==vs["official"]["sha256"],
            "no_likely_equal_fork":vs["fork_no_likely"]["sha256"]==vs["fork"]["sha256"],
            "official_size_bytes":vs["official"]["size_bytes"],
            "fork_size_bytes":vs["fork"]["size_bytes"],
            "no_likely_size_bytes":vs["fork_no_likely"]["size_bytes"],
            "fork_minus_official_bytes":vs["fork"]["size_bytes"]-vs["official"]["size_bytes"],
            "simd_degree":vs["fork"]["simd_degree"],
        })

    out={
        "schema":"RMR-LIKELY-POLICY-MATRIX-V2","claim_allowed":False,
        "digest_equivalence":"PASS","position_balance":"PASS",
        "details":details,"aggregate":aggregate,"structural":structural,
        "boundaries":[
            "PINNED_VCPU_REDUCES_SCHEDULER_VARIANCE_NOT_HYPERVISOR_VARIANCE",
            "RUNNER_OBSERVATION!=PHYSICAL_DEVICE_CLAIM",
            "SOURCE_DELTA!=BINARY_DELTA!=PERFORMANCE_DELTA",
            "SINGLE_RUN_POLICY_MATRIX!=UNIVERSAL_COMPILER_POLICY",
        ]
    }
    Path(a.json).write_text(json.dumps(out,indent=2,sort_keys=True)+"\n",encoding="utf-8")

    lines=["# RMR LIKELY policy matrix V2","",
           "CPU-pinned paired observations. claim_allowed=false.","",
           "| compiler | cap | comparison | bytes | Δ | 95% bootstrap | state | CV num/den |",
           "|---|---|---|---:|---:|---|---|---:|"]
    for r in details:
        lines.append(f"| {r['compiler']} | {r['cap']} | `{r['comparison']}` | {r['size_bytes']} | {r['paired_delta_percent']:+.3f}% | [{r['bootstrap_95_low']:.6f}, {r['bootstrap_95_high']:.6f}] | {r['classification']} | {r['numerator_cv_percent']:.2f}% / {r['denominator_cv_percent']:.2f}% |")
    lines += ["","## Aggregate descriptive medians","",
              "| compiler | cap | comparison | median Δ all paired points | n |",
              "|---|---|---|---:|---:|"]
    for r in aggregate:
        lines.append(f"| {r['compiler']} | {r['cap']} | `{r['comparison']}` | {r['paired_delta_percent_all_sizes']:+.3f}% | {r['observations']} |")
    lines += ["","## Structural equality","",
              "| compiler | cap | artifact | official=fork | no_likely=official | Δ bytes fork-official | SIMD degree |",
              "|---|---|---|---|---|---:|---:|"]
    for r in structural:
        lines.append(f"| {r['compiler']} | {r['cap']} | `{r['artifact']}` | {r['official_equal_fork']} | {r['no_likely_equal_official']} | {r['fork_minus_official_bytes']:+d} | {r['simd_degree']} |")
    lines += ["","SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM.",""]
    Path(a.md).write_text("\n".join(lines),encoding="utf-8")
    print(f"RMR_LIKELY_POLICY_ANALYSIS=PASS details={len(details)} aggregate={len(aggregate)} structural={len(structural)}")

if __name__=="__main__": main()
