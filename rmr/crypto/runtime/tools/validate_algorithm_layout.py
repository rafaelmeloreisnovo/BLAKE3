#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
import argparse, hashlib, json, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[4]
REGISTRY=ROOT/"rmr/crypto/runtime/registry.json"
LAYOUT=ROOT/"rmr/crypto/algorithms"
def fail(m): print("FAIL: "+m,file=sys.stderr); return 1
def main():
 p=argparse.ArgumentParser(); p.add_argument("--out"); a=p.parse_args()
 data=json.loads(REGISTRY.read_text(encoding="utf-8")); alg=data.get("algorithms",[])
 expected={x["id"] for x in alg}; actual={x.name for x in LAYOUT.iterdir() if x.is_dir()}
 if actual!=expected: return fail(f"directory mismatch missing={sorted(expected-actual)} extra={sorted(actual-expected)}")
 rows=[]
 for x in alg:
  f=LAYOUT/x["id"]/"README.md"
  if not f.is_file(): return fail(x["id"]+" missing README")
  t=f.read_text(encoding="utf-8")
  for k,v in {"algorithm_id":x["id"],"class":x["class"],"provider":x["provider"],"status":x["status"],"security_use":x["security_use"],"runtime_authority":"rmr/crypto/runtime/registry.json"}.items():
   if f"{k}: {v}" not in t: return fail(f"{x['id']} missing {k}: {v}")
  rows.append({"algorithm":x["id"],"readme_sha256":hashlib.sha256(f.read_bytes()).hexdigest()})
 legacy={x["id"]:x["security_use"] for x in alg if x["id"] in {"md5","sha1"}}
 if legacy!={"md5":"compatibility_only","sha1":"compatibility_only"}: return fail("legacy boundary changed")
 out={"schema":"RMR-CRYPTO-ALGORITHM-LAYOUT-V1","state":"PASS","claim_allowed":False,"algorithms":len(rows),"rows":rows}
 if a.out:
  q=Path(a.out); q.parent.mkdir(parents=True,exist_ok=True); q.write_text(json.dumps(out,indent=2,sort_keys=True)+"\n",encoding="utf-8")
 print(f"RMR_CRYPTO_ALGORITHM_LAYOUT=PASS algorithms={len(rows)}"); return 0
if __name__=="__main__": raise SystemExit(main())
