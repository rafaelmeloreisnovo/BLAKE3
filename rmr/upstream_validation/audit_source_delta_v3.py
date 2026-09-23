#!/usr/bin/env python3
from __future__ import annotations
import argparse, hashlib, json
from pathlib import Path

PERF_RELEVANT={
 "Cargo.toml","b3sum/src/main.rs","c/CMakeLists.txt","c/blake3.h",
 "c/blake3_dispatch.c","c/blake3_impl.h","c/blake3_neon.c",
 "c/blake3_neon_aarch64_unix.S","c/blake3_neon_armv7_unix.S"
}

def category(p:str)->str:
    if p.startswith("rmr/"): return "rmr"
    if p.startswith("audit") or p.startswith("audits/"): return "audit"
    if p.startswith("c/"): return "core-c"
    if p.startswith("b3sum/"): return "cli-rust"
    if p.startswith("src/") or p=="Cargo.toml": return "rust-core-build"
    if p.startswith(".github/") or p.startswith("tools/"): return "build-ci"
    if p.startswith("docs/") or p.endswith(".md"): return "docs"
    return "other"

def files(root:Path):
    out={}
    for p in root.rglob("*"):
        if not p.is_file() or ".git" in p.parts: continue
        rel=p.relative_to(root).as_posix()
        out[rel]=p
    return out

def sha(path:Path):
    h=hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda:f.read(1024*1024),b""): h.update(chunk)
    return h.hexdigest()

def text_lines(path:Path):
    try:
        return len(path.read_text(encoding="utf-8").splitlines())
    except (UnicodeDecodeError,OSError):
        return None

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--official",required=True)
    ap.add_argument("--fork",required=True)
    ap.add_argument("--out",required=True)
    args=ap.parse_args()
    a=files(Path(args.official)); b=files(Path(args.fork))
    allp=sorted(set(a)|set(b))
    rows=[]; counts={"added":0,"removed":0,"modified":0,"same":0}
    cats={}
    for rel in allp:
        if rel not in a:
            state="added"
        elif rel not in b:
            state="removed"
        else:
            state="same" if sha(a[rel])==sha(b[rel]) else "modified"
        counts[state]+=1
        cat=category(rel)
        cats.setdefault(cat,{"added":0,"removed":0,"modified":0,"same":0})
        cats[cat][state]+=1
        if state!="same":
            rows.append({
              "path":rel,"state":state,"category":cat,
              "performance_relevant":rel in PERF_RELEVANT,
              "official_sha256":sha(a[rel]) if rel in a else None,
              "fork_sha256":sha(b[rel]) if rel in b else None,
              "official_lines":text_lines(a[rel]) if rel in a else None,
              "fork_lines":text_lines(b[rel]) if rel in b else None
            })
    payload={
      "schema":"RMR-UPSTREAM-SOURCE-DELTA-V3",
      "claim_allowed":False,
      "counts":counts,"categories":cats,"changed":rows,
      "performance_relevant_changed":[r["path"] for r in rows if r["performance_relevant"]],
      "boundary":["CHANGED_SOURCE!=PERFORMANCE_GAIN","ADDED_RMR_FILE!=UPSTREAM_CORE_MODIFICATION"]
    }
    Path(args.out).write_text(json.dumps(payload,indent=2,sort_keys=True)+"\n",encoding="utf-8")
    print("RMR_SOURCE_DELTA_V3=PASS")
    print(json.dumps(counts,sort_keys=True))
    print("performance_relevant_changed="+str(len(payload["performance_relevant_changed"])))

if __name__=="__main__":
    main()
