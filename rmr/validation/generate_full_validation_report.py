#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

from __future__ import annotations

import argparse
import csv
import json
from pathlib import Path


def find_schema(root: Path, schema: str):
    for path in root.rglob("*.json"):
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
        except Exception:
            continue
        if data.get("schema") == schema:
            return path, data
    return None, None


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--artifacts", required=True)
    ap.add_argument("--json-out", required=True)
    ap.add_argument("--md-out", required=True)
    args = ap.parse_args()

    root = Path(args.artifacts)
    axes = []

    def add(axis, state, detail, evidence=None):
        axes.append({
            "axis": axis,
            "state": state,
            "detail": detail,
            "evidence": str(evidence) if evidence else None,
        })

    sanitizer_receipt = None
    for candidate in root.rglob("receipt.txt"):
        text = candidate.read_text(encoding="utf-8", errors="replace")
        if (
            "C_KAT_ASAN_UBSAN=PASS" in text
            and "C_INTRINSICS_AND_ASM_VECTORS=PASS" in text
        ):
            sanitizer_receipt = candidate
            break
    sanitizer_pass = sanitizer_receipt is not None
    add(
        "C-KAT-SAN",
        "PASS" if sanitizer_pass else "TOKEN_VAZIO_NOT_FOUND",
        "C known-answer vectors under ASan/UBSan for intrinsics and assembly."
        if sanitizer_pass else "No explicit sanitizer PASS receipt.",
        sanitizer_receipt,
    )

    p, upstream = find_schema(root, "RMR-BLAKE3-UPSTREAM-COMPARE-V2")
    if upstream:
        classes = [x["classification"] for x in upstream["analysis"]]
        add(
            "C-UPSTREAM-V2",
            "PASS",
            f"{len(classes)} size classes; classifications={classes}",
            p,
        )
    else:
        add("C-UPSTREAM-V2", "TOKEN_VAZIO_NOT_FOUND", "No V2 comparison receipt.")

    p, ablation = find_schema(root, "RMR-C-ABLATION-V3")
    if not ablation:
        p, ablation = find_schema(root, "RMR-C-ABLATION-V1")
    add(
        "C-ABLATION",
        "PASS" if ablation else "TOKEN_VAZIO_NOT_FOUND",
        "fork/no-likely/no-restrict/no-hints causal matrix with digest equivalence"
        if ablation else "No ablation receipt.",
        p,
    )

    p, simd = find_schema(root, "RMR-BLAKE3-BACKEND-MATRIX-V3")
    if not simd:
        p, simd = find_schema(root, "RMR-SIMD-BACKEND-MATRIX-V1")
    add(
        "SIMD-X86",
        "PASS" if simd else "TOKEN_VAZIO_NOT_FOUND",
        "official-vs-fork portable/SSE2/SSE4.1/AVX2/auto backend matrix"
        if simd else "No SIMD/backend receipt.",
        p,
    )

    p, rust = find_schema(root, "RMR-RUST-LIBRARY-V3")
    if not rust:
        p, rust = find_schema(root, "RMR-RUST-LTO-COMPARE-V1")
    add(
        "RUST-LTO",
        "PASS" if rust else "TOKEN_VAZIO_NOT_FOUND",
        "upstream/fork plus release-profile/LTO ablation"
        if rust else "No Rust receipt.",
        p,
    )

    p, b3sum = find_schema(root, "RMR-RUST-CLI-V3")
    if not b3sum:
        p, b3sum = find_schema(root, "RMR-B3SUM-COMPARE-V1")
    add(
        "B3SUM",
        "PASS" if b3sum else "TOKEN_VAZIO_NOT_FOUND",
        "hash, threaded hash, mmap/no-mmap/checkfile-oriented CLI workloads"
        if b3sum else "No b3sum receipt.",
        p,
    )

    p, binary = find_schema(root, "RMR-BINARY-SURFACE-AUDIT-V1")
    if binary:
        state = "PASS" if (
            binary.get("public_api_contract") == "PASS"
            and binary.get("fork_gnu_stack_gate") == "PASS"
        ) else "REVIEW"
        add(
            "BINARY",
            state,
            f"public_api={binary.get('public_api_contract')} "
            f"gnu_stack={binary.get('fork_gnu_stack_gate')} "
            f"warnings={binary.get('strict_warning_state')}",
            p,
        )
    else:
        add("BINARY", "TOKEN_VAZIO_NOT_FOUND", "No binary audit receipt.")

    cross_csv = None
    rows = []
    for candidate in root.rglob("status.csv"):
        try:
            candidate_rows = list(csv.DictReader(candidate.open(encoding="utf-8")))
        except Exception:
            continue
        if (
            candidate_rows
            and {"profile", "state", "target"}.issubset(candidate_rows[0])
        ):
            cross_csv = candidate
            rows = candidate_rows
            break
    if cross_csv:
        states = [r.get("state", "") for r in rows]
        cross_pass = bool(rows) and all(state == "PASS" for state in states)
        add(
            "CROSS-ARCH",
            "PASS" if cross_pass else "TOKEN_VAZIO_TOOLCHAIN",
            "; ".join(f"{r.get('profile')}={r.get('state')}" for r in rows),
            cross_csv,
        )
    else:
        add("CROSS-ARCH", "TOKEN_VAZIO_NOT_FOUND", "No cross-architecture receipt.")

    add(
        "ARM-PHYSICAL",
        "TOKEN_VAZIO_PHYSICAL",
        "CI cross-compilation is not physical ARMv7/AArch64 execution.",
    )
    add(
        "IOPS-PHYSICAL",
        "TOKEN_VAZIO_CURRENT_CAMPAIGN",
        "Physical storage performance requires device/workload receipt.",
    )
    add(
        "INDEPENDENT-REPRODUCTION",
        "TOKEN_VAZIO",
        "No independent third-party reproduction was created by this workflow.",
    )

    required_ci_axes = {
        "C-KAT-SAN",
        "C-UPSTREAM-V2",
        "C-ABLATION",
        "SIMD-X86",
        "RUST-LTO",
        "B3SUM",
        "BINARY",
        "CROSS-ARCH",
    }
    missing_required = [
        row["axis"] for row in axes
        if row["axis"] in required_ci_axes
        and row["state"].startswith("TOKEN_VAZIO")
    ]
    review_required = [
        row["axis"] for row in axes
        if row["axis"] in required_ci_axes and row["state"] == "REVIEW"
    ]
    execution_complete = not missing_required
    validation_state = (
        "PARTIAL" if missing_required
        else "COMPLETE_WITH_REVIEW" if review_required
        else "COMPLETE"
    )

    result = {
        "schema": "RMR-BLAKE3-FULL-VALIDATION-RECEIPT-V2",
        "claim_allowed": False,
        "report_generation_state": "PASS",
        "execution_complete": execution_complete,
        "validation_state": validation_state,
        "missing_required_axes": missing_required,
        "review_required_axes": review_required,
        "axes": axes,
        "boundaries": [
            "FORK!=UPSTREAM",
            "CORE!=CLI!=RMR_PIPELINE",
            "CI!=PHYSICAL_DEVICE",
            "COMPILE!=EXECUTION",
            "OBSERVED_DELTA!=UNIVERSAL_SUPERIORITY",
        ],
    }

    Path(args.json_out).write_text(
        json.dumps(result, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )

    lines = [
        "# RMR BLAKE3 Full Validation Receipt V2",
        "",
        "SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM",
        "",
        "| Axis | State | Evidence summary |",
        "| --- | --- | --- |",
    ]
    for row in axes:
        lines.append(
            f"| {row['axis']} | {row['state']} | {row['detail'].replace('|', '/')} |"
        )
    lines += [
        "",
        "",
        f"CI execution complete: {str(execution_complete).lower()}",
        f"Validation state: {validation_state}",
        f"Missing required axes: {', '.join(missing_required) if missing_required else 'none'}",
        f"Review-required axes: {', '.join(review_required) if review_required else 'none'}",
        "",
        "Physical ARM and physical IOPS remain TOKEN_VAZIO unless device-bound",
        "receipts exist. No repository-wide superiority claim is emitted.",
        "",
    ]
    Path(args.md_out).write_text("\n".join(lines), encoding="utf-8")

    print("RMR_FULL_VALIDATION_REPORT_GENERATION=PASS")
    print("RMR_FULL_VALIDATION_EXECUTION_COMPLETE=" + ("true" if execution_complete else "false"))
    print("RMR_FULL_VALIDATION_STATE=" + validation_state)
    if missing_required:
        print("missing_required_axes=" + ",".join(missing_required))
    if review_required:
        print("review_required_axes=" + ",".join(review_required))
    for row in axes:
        print(f"{row['axis']}={row['state']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
