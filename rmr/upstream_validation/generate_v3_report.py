#!/usr/bin/env python3
from __future__ import annotations
import argparse, json
from pathlib import Path

SCHEMAS = {
 "source":"RMR-UPSTREAM-SOURCE-DELTA-V3",
 "core":"RMR-BLAKE3-UPSTREAM-COMPARE-V2",
 "backends":"RMR-BLAKE3-BACKEND-MATRIX-V3",
 "ablation":"RMR-C-ABLATION-V3",
 "abi":"RMR-BLAKE3-ABI-ELF-V3",
 "tbb":"RMR-TBB-V3",
 "rustlib":"RMR-RUST-LIBRARY-V3",
 "rustcli":"RMR-RUST-CLI-V3",
 "cross":"RMR-CROSS-ARCH-V3",
}

def find_schema(root:Path,schema:str):
    for p in root.rglob("*.json"):
        try:
            d=json.loads(p.read_text(encoding="utf-8"))
        except Exception:
            continue
        if d.get("schema")==schema:
            return p,d
    return None,None

def find_text(root:Path,needle:str):
    for p in root.rglob("*.txt"):
        try: s=p.read_text(encoding="utf-8",errors="replace")
        except Exception: continue
        if needle in s: return p,s
    return None,None

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--artifacts",required=True)
    ap.add_argument("--json-out",required=True)
    ap.add_argument("--md-out",required=True)
    a=ap.parse_args()
    root=Path(a.artifacts)
    axes=[]

    def add(axis,state,detail,evidence=None):
        axes.append({"axis":axis,"state":state,"detail":detail,
                     "evidence":str(evidence) if evidence else None})

    p,d=find_schema(root,SCHEMAS["source"])
    if d:
        add("SOURCE-PROVENANCE","PASS",
            f"changed={len(d['changed'])} performance_relevant={len(d['performance_relevant_changed'])}",p)
    else:add("SOURCE-PROVENANCE","TOKEN_VAZIO_NOT_FOUND","source receipt missing")

    p,s=find_text(root,"RMR_C_QUALITY_V3=PASS")
    add("C-QUALITY","PASS" if s else "TOKEN_VAZIO_NOT_FOUND",
        "strict warnings + CTest + semantic/alignment equivalence + ASan/UBSan + install consumer" if s else "receipt missing",p)

    p,d=find_schema(root,SCHEMAS["core"])
    if d:
        classes=[x["classification"] for x in d["analysis"]]
        add("C-CORE-PERFORMANCE","PASS",
            f"{len(classes)} size classes; classifications={classes}",p)
    else:add("C-CORE-PERFORMANCE","TOKEN_VAZIO_NOT_FOUND","core receipt missing")

    p,d=find_schema(root,SCHEMAS["ablation"])
    add("C-ABLATION","PASS" if d else "TOKEN_VAZIO_NOT_FOUND",
        "fork/no-likely/no-restrict/no-hints causal matrix" if d else "receipt missing",p)

    p,d=find_schema(root,SCHEMAS["backends"])
    add("SIMD-BACKENDS","PASS" if d else "TOKEN_VAZIO_NOT_FOUND",
        f"rows={len(d['rows'])}" if d else "receipt missing",p)

    p,d=find_schema(root,SCHEMAS["tbb"])
    add("C-TBB","PASS" if d else "TOKEN_VAZIO_NOT_FOUND",
        f"rows={len(d['rows'])}" if d else "receipt missing",p)

    p,d=find_schema(root,SCHEMAS["abi"])
    if d:
        missing=d.get("symbols_only_official",[])
        extra=d.get("symbols_only_fork",[])
        state="PASS" if (
            d.get("gnu_stack",{}).get("fork")=="PASS_NON_EXECUTABLE"
            and d.get("public_abi_probe_equal") is True
            and not missing
            and not extra
        ) else "REVIEW"
        add("ABI-ELF",state,
            f"abi_equal={d.get('public_abi_probe_equal')} "
            f"symbols_missing={len(missing)} symbols_fork_extra={len(extra)}",p)
    else:add("ABI-ELF","TOKEN_VAZIO_NOT_FOUND","receipt missing")

    p,d=find_schema(root,SCHEMAS["rustlib"])
    add("RUST-LIBRARY","PASS" if d else "TOKEN_VAZIO_NOT_FOUND",
        f"rows={len(d['rows'])}" if d else "receipt missing",p)

    p,d=find_schema(root,SCHEMAS["rustcli"])
    add("B3SUM-CLI","PASS" if d else "TOKEN_VAZIO_NOT_FOUND",
        f"rows={len(d['rows'])}" if d else "receipt missing",p)

    p,d=find_schema(root,SCHEMAS["cross"])
    add("CROSS-ARCH","PASS" if d else "TOKEN_VAZIO_NOT_FOUND",
        f"targets={d.get('targets')}" if d else "receipt missing",p)

    p,s=find_text(root,"RMR_INTEGRATION_V3=PASS")
    add("RMR-INTEGRATION","PASS" if s else "TOKEN_VAZIO_NOT_FOUND",
        "topology/HWIF/crypto/SIMPERF/fixed256/freestanding/PAI42 integration" if s else "receipt missing",p)

    add("ARM-PHYSICAL","TOKEN_VAZIO_PHYSICAL","cross compilation is not ARMv7/AArch64 physical execution")
    add("INDEPENDENT-REPRODUCTION","TOKEN_VAZIO","no independent third-party receipt created by CI")

    required_ci_axes={
      "SOURCE-PROVENANCE","C-QUALITY","C-CORE-PERFORMANCE","C-ABLATION",
      "SIMD-BACKENDS","C-TBB","ABI-ELF","RUST-LIBRARY","B3SUM-CLI",
      "CROSS-ARCH","RMR-INTEGRATION"
    }
    missing_required=[
      x["axis"] for x in axes
      if x["axis"] in required_ci_axes and x["state"].startswith("TOKEN_VAZIO")
    ]
    review_required=[
      x["axis"] for x in axes
      if x["axis"] in required_ci_axes and x["state"]=="REVIEW"
    ]
    execution_complete=not missing_required
    validation_state=(
      "PARTIAL" if missing_required
      else "COMPLETE_WITH_REVIEW" if review_required
      else "COMPLETE"
    )

    payload={
      "schema":"RMR-UPSTREAM-COMPREHENSIVE-V4",
      "claim_allowed":False,
      "report_generation_state":"PASS",
      "execution_complete":execution_complete,
      "validation_state":validation_state,
      "missing_required_axes":missing_required,
      "review_required_axes":review_required,
      "axes":axes,
      "boundaries":[
        "SOURCE!=BUILD!=EXECUTION!=EVIDENCE!=CLAIM",
        "CORE_GAIN!=CLI_GAIN","X86_GAIN!=ARM_GAIN","SIMD_GAIN!=TBB_GAIN",
        "RMR_PIPELINE_GAIN!=BLAKE3_PRIMITIVE_GAIN","COMPILE_PASS!=PHYSICAL_EXECUTION"
      ]
    }
    Path(a.json_out).write_text(json.dumps(payload,indent=2,sort_keys=True)+"\n",encoding="utf-8")
    lines=[
      "# RMR upstream comprehensive validation V4",
      "",
      "SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM",
      "",
      "| Axis | State | Detail |",
      "| --- | --- | --- |",
    ]
    for x in axes:
        lines.append(f"| {x['axis']} | {x['state']} | {x['detail'].replace('|','/')} |")
    lines += [
      "",
      f"CI execution complete: {str(execution_complete).lower()}",
      f"Validation state: {validation_state}",
      f"Missing required axes: {', '.join(missing_required) if missing_required else 'none'}",
      f"Review-required axes: {', '.join(review_required) if review_required else 'none'}",
      "",
      "No repository-wide speed claim is emitted by this receipt.",
      ""
    ]
    Path(a.md_out).write_text("\n".join(lines),encoding="utf-8")
    print("RMR_UPSTREAM_COMPREHENSIVE_REPORT_GENERATION=PASS")
    print("RMR_UPSTREAM_COMPREHENSIVE_EXECUTION_COMPLETE=" + ("true" if execution_complete else "false"))
    print("RMR_UPSTREAM_COMPREHENSIVE_STATE=" + validation_state)
    if missing_required:
        print("missing_required_axes=" + ",".join(missing_required))
    if review_required:
        print("review_required_axes=" + ",".join(review_required))
    for x in axes: print(f"{x['axis']}={x['state']}")

if __name__=="__main__":
    main()
