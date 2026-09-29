#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
from __future__ import annotations
import argparse,csv,json,math,random,statistics
from collections import Counter,defaultdict
from pathlib import Path

VARIANTS=("official","fork","fork_no_likely","fork_no_restrict","fork_no_hints")
COMPARISONS=(
    ("fork_over_official","fork","official"),
    ("no_likely_over_fork","fork_no_likely","fork"),
    ("no_restrict_over_fork","fork_no_restrict","fork"),
    ("no_hints_over_fork","fork_no_hints","fork"),
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
    if not rows: raise SystemExit("results=EMPTY")
    required={"variant","size_bytes","round","position","iterations","seconds","ns_per_op","mib_s","digest"}
    for i,r in enumerate(rows,start=2):
        if set(r)!=required or None in r or any(r[k] in (None,"") for k in required):
            raise SystemExit(f"results_shape=FAIL line={i}")
        if r["variant"] not in VARIANTS: raise SystemExit(f"variant=FAIL line={i}")

    grouped=defaultdict(lambda:defaultdict(dict)); positions=defaultdict(Counter); digest_by_size=defaultdict(set)
    for r in rows:
        size=int(r["size_bytes"]); rd=int(r["round"]); v=r["variant"]
        grouped[size][rd][v]=float(r["mib_s"])
        positions[v][int(r["position"])]+=1
        digest_by_size[size].add(r["digest"])
    for size,d in digest_by_size.items():
        if len(d)!=1: raise SystemExit(f"digest_equivalence=FAIL size={size}")
    for size,rounds in grouped.items():
        for rd,p in rounds.items():
            if set(p)!=set(VARIANTS): raise SystemExit(f"round_completeness=FAIL size={size} round={rd}")
    for v,c in positions.items():
        vals=list(c.values())
        if set(c)!=set(range(5)) or max(vals)!=min(vals):
            raise SystemExit(f"position_balance=FAIL variant={v} counts={dict(c)}")

    details=[]
    for size in sorted(grouped):
        rounds=grouped[size]
        for label,num,den in COMPARISONS:
            pairs=[(rounds[rd][num],rounds[rd][den]) for rd in sorted(rounds)]
            ratios=[x/y for x,y in pairs]
            seed=(size ^ sum(map(ord,label))) & 0xffffffff
            lo,hi=bootstrap(pairs,seed)
            numxs=[x for x,_ in pairs]; denxs=[y for _,y in pairs]
            details.append({
                "comparison":label,"size_bytes":size,"rounds":len(pairs),
                "numerator":num,"denominator":den,
                "numerator_median_mib_s":statistics.median(numxs),
                "denominator_median_mib_s":statistics.median(denxs),
                "paired_ratio_median":statistics.median(ratios),
                "paired_delta_percent":(statistics.median(ratios)-1.0)*100.0,
                "bootstrap_95_low":lo,"bootstrap_95_high":hi,
                "classification":classify(lo,hi),
                "numerator_cv_percent":cv(numxs),"denominator_cv_percent":cv(denxs),
            })

    objrows=list(csv.DictReader(Path(a.objects).open(encoding="utf-8")))
    om=defaultdict(dict)
    for r in objrows:
        om[r["artifact"]][r["variant"]]={"sha256":r["sha256"],"size_bytes":int(r["size_bytes"])}
    structural=[]
    for art,vs in sorted(om.items()):
        if not set(VARIANTS).issubset(vs): raise SystemExit(f"object_matrix=FAIL artifact={art}")
        base=vs["fork"]
        structural.append({
            "artifact":art,
            "official_equal_fork":vs["official"]["sha256"]==base["sha256"],
            "no_likely_equal_fork":vs["fork_no_likely"]["sha256"]==base["sha256"],
            "no_restrict_equal_fork":vs["fork_no_restrict"]["sha256"]==base["sha256"],
            "no_hints_equal_fork":vs["fork_no_hints"]["sha256"]==base["sha256"],
            "official_size_bytes":vs["official"]["size_bytes"],
            "fork_size_bytes":base["size_bytes"],
            "fork_minus_official_bytes":base["size_bytes"]-vs["official"]["size_bytes"],
            "hashes":vs,
        })

    out={
        "schema":"RMR-GCC-SSE41-CAUSAL-V1","claim_allowed":False,
        "digest_equivalence":"PASS","position_balance":"PASS",
        "details":details,"structural":structural,
        "boundaries":[
            "PAIRED_ABLATION_OBSERVATION!=UNIVERSAL_CAUSAL_CLAIM",
            "OBJECT_HASH_DELTA!=PERFORMANCE_CAUSALITY",
            "RUNNER_OBSERVATION!=PHYSICAL_DEVICE_CLAIM",
            "SOURCE_DELTA!=BINARY_DELTA!=PERFORMANCE_DELTA",
        ]
    }
    Path(a.json).write_text(json.dumps(out,indent=2,sort_keys=True)+"\n",encoding="utf-8")

    lines=["# RMR GCC SSE4.1 causal delta V1","",
           "Paired cyclic-position experiment. claim_allowed=false.","",
           "| comparison | bytes | Δ | 95% bootstrap | state | CV num/den |",
           "|---|---:|---:|---|---|---:|"]
    for r in details:
        lines.append(f"| `{r['comparison']}` | {r['size_bytes']} | {r['paired_delta_percent']:+.3f}% | [{r['bootstrap_95_low']:.6f}, {r['bootstrap_95_high']:.6f}] | {r['classification']} | {r['numerator_cv_percent']:.2f}% / {r['denominator_cv_percent']:.2f}% |")
    lines += ["","## Structural equality against fork","",
              "| artifact | official=fork | no_likely=fork | no_restrict=fork | no_hints=fork | Δ bytes fork-official |",
              "|---|---|---|---|---|---:|"]
    for r in structural:
        lines.append(f"| `{r['artifact']}` | {r['official_equal_fork']} | {r['no_likely_equal_fork']} | {r['no_restrict_equal_fork']} | {r['no_hints_equal_fork']} | {r['fork_minus_official_bytes']:+d} |")
    lines += ["","SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM.",""]
    Path(a.md).write_text("\n".join(lines),encoding="utf-8")
    print(f"RMR_GCC_SSE41_CAUSAL_ANALYSIS=PASS comparisons={len(details)} artifacts={len(structural)}")

if __name__=="__main__": main()
