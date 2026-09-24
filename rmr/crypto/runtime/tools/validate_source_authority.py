#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
import argparse,json,re,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[4]
EXPECTED="https://github.com/BLAKE3-team/BLAKE3.git"
P={"common":ROOT/"rmr/upstream_validation/common.sh","v1":ROOT/"rmr/tools/orchestrate_blake3_compare.sh","v2":ROOT/"rmr/tools/orchestrate_blake3_compare_v2.sh","workflow":ROOT/".github/workflows/rmr-upstream-comprehensive-v3.yml","cmake":ROOT/"rmr/CMakeLists.txt","adapter":ROOT/"rmr/crypto/runtime/src/rmr_crypto_blake3.c","registry":ROOT/"rmr/crypto/runtime/registry.json"}
def fail(m): print("FAIL: "+m,file=sys.stderr); return 1
def val(t,n):
 m=re.search(rf'{n}="\$\{{{n}:-([^}}]+)\}}"',t)
 if not m: raise ValueError("missing "+n)
 return m.group(1)
def main():
 p=argparse.ArgumentParser(); p.add_argument("--out"); a=p.parse_args()
 try:
  t={k:v.read_text(encoding="utf-8") for k,v in P.items() if k!="registry"}
  repos={k:val(t[k],"OFFICIAL_REPO") for k in ("common","v1","v2")}
  refs={k:val(t[k],"OFFICIAL_REF") for k in ("common","v1","v2")}
 except (OSError,ValueError) as e: return fail(str(e))
 if set(repos.values())!={EXPECTED}: return fail(f"official repository mismatch {repos}")
 if len(set(refs.values()))!=1: return fail(f"OFFICIAL_REF mismatch {refs}")
 m=re.search(r"(?m)^\s*OFFICIAL_REF:\s*([0-9a-f]{40})\s*$",t["workflow"])
 if not m: return fail("workflow missing OFFICIAL_REF")
 ref=next(iter(refs.values()))
 if m.group(1)!=ref: return fail("workflow/script OFFICIAL_REF mismatch")
 marker='add_subdirectory("'+"$"+'{CMAKE_CURRENT_SOURCE_DIR}/../c"'
 if marker not in t["cmake"]: return fail("normal RMR build no longer uses repository ../c")
 if '#include "blake3.h"' not in t["adapter"]: return fail("BLAKE3 adapter API drift")
 if any(x in t["adapter"] for x in ("http://","https://","git clone")): return fail("network source logic in runtime adapter")
 reg=json.loads(P["registry"].read_text(encoding="utf-8")); b=next((x for x in reg["algorithms"] if x["id"]=="blake3"),None)
 if not b or b.get("provider")!="BLAKE3-upstream": return fail("registry BLAKE3 authority drift")
 out={"schema":"RMR-BLAKE3-SOURCE-AUTHORITY-V1","state":"PASS","claim_allowed":False,"fork_runtime_source":"current_repository_checkout:c/","official_repository":EXPECTED,"official_ref":ref}
 if a.out:
  q=Path(a.out); q.parent.mkdir(parents=True,exist_ok=True); q.write_text(json.dumps(out,indent=2,sort_keys=True)+"\n",encoding="utf-8")
 print("RMR_BLAKE3_SOURCE_AUTHORITY=PASS"); print("official_repository="+EXPECTED); print("official_ref="+ref); print("fork_runtime_source=current_repository_checkout:c/"); return 0
if __name__=="__main__": raise SystemExit(main())
