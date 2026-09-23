#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# RMR analytical performance-profile emulator.
# It is deliberately NOT an ISA/cycle-accurate emulator.

from __future__ import annotations

import argparse
import csv
import json
import math
from pathlib import Path

DEFAULT_SIZES = [1, 16, 64, 256, 1024, 4096, 16384, 65536, 1048576, 16777216]


def load_profiles(path: Path) -> dict[str, dict]:
    data = json.loads(path.read_text(encoding="utf-8"))
    return {p["id"]: p for p in data["profiles"]}


def load_measurements(path: Path) -> dict[str, list[dict]]:
    rows: dict[str, list[dict]] = {}
    with path.open(newline="", encoding="utf-8") as f:
        for raw in csv.DictReader(f):
            row = dict(raw)
            row["size_bytes"] = int(row["size_bytes"])
            row["ns_per_op"] = float(row["ns_per_op"])
            rows.setdefault(row["algorithm"], []).append(row)
    for alg in rows:
        rows[alg].sort(key=lambda x: x["size_bytes"])
    return rows


def log_interp(rows: list[dict], size: int, field: str) -> float:
    if not rows:
        raise ValueError("no calibration rows")
    exact = next((r for r in rows if r["size_bytes"] == size), None)
    if exact:
        return float(exact[field])
    if size <= rows[0]["size_bytes"]:
        return float(rows[0][field])
    if size >= rows[-1]["size_bytes"]:
        return float(rows[-1][field])

    x = math.log(max(size, 1))
    for left, right in zip(rows, rows[1:]):
        if left["size_bytes"] < size < right["size_bytes"]:
            x0 = math.log(max(left["size_bytes"], 1))
            x1 = math.log(max(right["size_bytes"], 1))
            t = (x - x0) / (x1 - x0)
            y0 = math.log(max(float(left[field]), 1e-12))
            y1 = math.log(max(float(right[field]), 1e-12))
            return math.exp(y0 + t * (y1 - y0))
    raise AssertionError("unreachable interpolation")


def compute_capacity(profile: dict) -> float:
    # Relative compute capacity only; absolute performance comes from calibration.
    return (
        float(profile["clock_ghz"])
        * float(profile["vector_bytes"])
        * float(profile["issue_factor"])
    )


def bytes_per_ns_from_measurement(size: int, ns: float) -> float:
    return float(size) / max(ns, 1e-12)


def predict_official_ns(size: int, ref_ns: float, ref: dict, target: dict) -> float:
    if size <= 0:
        return max(float(target["startup_ns"]), 1e-9)

    ref_rate = bytes_per_ns_from_measurement(size, ref_ns)
    compute_scale = compute_capacity(target) / max(compute_capacity(ref), 1e-12)
    compute_rate = ref_rate * compute_scale

    # GB/s numerically equals bytes/ns under decimal SI.
    memory_rate = float(target["memory_gbps"]) * float(target["memory_efficiency"])
    effective_rate = max(min(compute_rate, memory_rate), 1e-12)

    ref_startup = float(ref["startup_ns"])
    target_startup = float(target["startup_ns"])
    ref_payload_ns = max(ref_ns - ref_startup, size / max(ref_rate, 1e-12))
    scaled_payload_ns = max(size / effective_rate, ref_payload_ns / max(compute_scale, 1e-12))

    return target_startup + scaled_payload_ns


def candidate_speedup(official_rows: list[dict], candidate_rows: list[dict], size: int) -> float:
    official_ns = log_interp(official_rows, size, "ns_per_op")
    candidate_ns = log_interp(candidate_rows, size, "ns_per_op")
    return official_ns / max(candidate_ns, 1e-12)


def interval(value: float, relative_uncertainty: float) -> tuple[float, float]:
    low = max(value * (1.0 - relative_uncertainty), 0.0)
    high = value * (1.0 + relative_uncertainty)
    return low, high


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--profiles", required=True)
    ap.add_argument("--official", required=True, help="CSV: algorithm,size_bytes,ns_per_op")
    ap.add_argument("--candidate", help="optional RMR/candidate CSV with same schema")
    ap.add_argument("--reference-profile", required=True)
    ap.add_argument("--target-profile", action="append", default=[])
    ap.add_argument("--sizes", default=",".join(str(x) for x in DEFAULT_SIZES))
    ap.add_argument("--transfer", type=float, default=0.70,
                    help="fraction of observed candidate speedup transferred cross-profile")
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    profiles = load_profiles(Path(args.profiles))
    official = load_measurements(Path(args.official))
    candidate = load_measurements(Path(args.candidate)) if args.candidate else {}

    if args.reference_profile not in profiles:
        raise SystemExit("unknown reference profile")
    ref = profiles[args.reference_profile]
    targets = args.target_profile or list(profiles)
    sizes = [int(x) for x in args.sizes.split(",") if x.strip()]

    output = []
    for target_id in targets:
        target = profiles[target_id]
        for algorithm, official_rows in sorted(official.items()):
            for size in sizes:
                ref_ns = log_interp(official_rows, size, "ns_per_op")
                official_ns = predict_official_ns(size, ref_ns, ref, target)

                speedup_ref = None
                candidate_ns = None
                if algorithm in candidate:
                    speedup_ref = candidate_speedup(
                        official_rows, candidate[algorithm], size
                    )
                    transferred = 1.0 + ((speedup_ref - 1.0) * args.transfer)
                    transferred = max(transferred, 0.05)
                    candidate_ns = official_ns / transferred

                rel_u = float(target["relative_uncertainty"])
                o_low, o_high = interval(official_ns, rel_u)
                row = {
                    "algorithm": algorithm,
                    "size_bytes": size,
                    "reference_profile": args.reference_profile,
                    "target_profile": target_id,
                    "model_state": "ANALOGY_ESTIMATE",
                    "official_est_ns_per_op": official_ns,
                    "official_est_ns_low": o_low,
                    "official_est_ns_high": o_high,
                    "official_est_ops_per_s": 1e9 / max(official_ns, 1e-12),
                    "official_est_mib_per_s": (
                        size / max(official_ns, 1e-12) * 1e9 / (1024 * 1024)
                    ),
                    "claim_allowed": False,
                }

                if candidate_ns is not None:
                    c_low, c_high = interval(candidate_ns, rel_u + 0.10)
                    row.update({
                        "reference_observed_speedup": speedup_ref,
                        "transferred_speedup": official_ns / candidate_ns,
                        "candidate_est_ns_per_op": candidate_ns,
                        "candidate_est_ns_low": c_low,
                        "candidate_est_ns_high": c_high,
                        "candidate_est_ops_per_s": 1e9 / max(candidate_ns, 1e-12),
                        "candidate_est_mib_per_s": (
                            size / max(candidate_ns, 1e-12) * 1e9 / (1024 * 1024)
                        ),
                    })

                output.append(row)

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({
        "schema": "RMR-SIMPERF-V1",
        "claim_allowed": False,
        "model_kind": "calibrated_analytical_profile_emulator",
        "not_cycle_accurate": True,
        "reference_profile": args.reference_profile,
        "transfer": args.transfer,
        "rows": output,
        "boundaries": [
            "SIMULATION!=PHYSICAL_BENCHMARK",
            "ANALOGY_ESTIMATE!=MEASUREMENT",
            "OFFICIAL_CALIBRATION!=RMR_SUPERIORITY_CLAIM",
            "CROSS_PROFILE_TRANSFER!=CROSS_DEVICE_EVIDENCE"
        ]
    }, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"RMR_SIMPERF=PASS rows={len(output)} out={out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
