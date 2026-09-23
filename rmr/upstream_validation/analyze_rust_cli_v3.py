#!/usr/bin/env python3
import csv,json,statistics,sys
from collections import defaultdict
from pathlib import Path

def main():
    rows=list(csv.DictReader(Path(sys.argv[1]).open(encoding="utf-8")))
    g=defaultdict(list)
    for r in rows:
        g[(r["binary"],r["workload"])].append(float(r["seconds"]))
    summary=[]
    for (binary,workload),vals in sorted(g.items()):
        summary.append({
          "binary":binary,"workload":workload,"rounds":len(vals),
          "median_seconds":statistics.median(vals),
          "mean_seconds":statistics.fmean(vals),
          "cv_percent":(statistics.stdev(vals)/statistics.fmean(vals)*100.0) if len(vals)>1 else 0.0
        })
    out={"schema":"RMR-RUST-CLI-V3","claim_allowed":False,"rows":summary,
         "boundary":["CLI_TIME!=CORE_HASH_TIME","LTO_ABLATION!=UPSTREAM_DIFF"]}
    Path(sys.argv[2]).write_text(json.dumps(out,indent=2,sort_keys=True)+"\n")
    print("RMR_RUST_CLI_ANALYSIS=PASS")
    for r in summary:
        print(f"{r['binary']} {r['workload']} median={r['median_seconds']:.6f}s cv={r['cv_percent']:.2f}%")
if __name__=="__main__": main()
