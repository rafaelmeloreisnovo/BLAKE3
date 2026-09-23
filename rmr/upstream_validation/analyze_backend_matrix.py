#!/usr/bin/env python3
import csv, json, statistics, sys
from collections import defaultdict
from pathlib import Path

def main():
    src=Path(sys.argv[1])
    out=Path(sys.argv[2])
    rows=list(csv.DictReader(src.open(encoding="utf-8")))
    g=defaultdict(lambda: defaultdict(list))
    dig=defaultdict(set)
    degree=defaultdict(set)
    for r in rows:
        key=(r["variant"],int(r["size_bytes"]))
        side=r["side"]
        g[key][side].append(float(r["mib_s"]))
        dig[(key,side)].add(r["digest"])
        degree[(r["variant"],side)].add(int(r["simd_degree"]))
    result=[]
    for key in sorted(g):
        variant,size=key
        if set(g[key])!={"official","fork"}:
            raise SystemExit(f"incomplete {key}")
        if len(dig[(key,"official")])!=1 or len(dig[(key,"fork")])!=1:
            raise SystemExit(f"unstable digest {key}")
        if dig[(key,"official")] != dig[(key,"fork")]:
            raise SystemExit(f"digest mismatch {key}")
        o=statistics.median(g[key]["official"])
        f=statistics.median(g[key]["fork"])
        result.append({
            "variant":variant,"size_bytes":size,
            "official_median_mib_s":o,"fork_median_mib_s":f,
            "fork_over_official":f/o,
            "delta_percent":(f/o-1.0)*100.0,
            "digest_equivalence":"PASS"
        })
    payload={
      "schema":"RMR-BLAKE3-BACKEND-MATRIX-V3",
      "claim_allowed":False,
      "simd_degrees":{"%s/%s"%k:sorted(v) for k,v in degree.items()},
      "rows":result,
      "boundary":["FORCED_BACKEND_RESULT!=UNIVERSAL_CPU_RESULT","SIMD_DEGREE!=ISA_NAME"]
    }
    out.write_text(json.dumps(payload,indent=2,sort_keys=True)+"\n",encoding="utf-8")
    print(f"RMR_BACKEND_MATRIX_ANALYSIS=PASS rows={len(result)}")
    for r in result:
        print(f"{r['variant']} size={r['size_bytes']} delta={r['delta_percent']:+.3f}%")

if __name__=="__main__":
    main()
