#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

from __future__ import annotations
import argparse
import json
from pathlib import Path


def parse(path: Path) -> dict[str, str]:
    out = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        if "=" not in line:
            continue
        k, v = line.split("=", 1)
        out[k] = v
    return out


def as_float(v: str | None):
    try:
        return float(v) if v not in (None, "") else None
    except ValueError:
        return None


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--before", required=True)
    ap.add_argument("--after", required=True)
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    before = parse(Path(args.before))
    after = parse(Path(args.after))

    freq = []
    thermal = []

    keys = sorted(set(before) | set(after))
    for key in keys:
        if key.endswith(".scaling_cur_freq") or key.endswith(".cpuinfo_cur_freq"):
            a = as_float(before.get(key))
            b = as_float(after.get(key))
            if a is not None and b is not None and a > 0:
                freq.append({
                    "key": key,
                    "before": a,
                    "after": b,
                    "delta_percent": (b / a - 1.0) * 100.0,
                })
        if key.endswith(".temp"):
            a = as_float(before.get(key))
            b = as_float(after.get(key))
            if a is not None and b is not None:
                thermal.append({
                    "key": key,
                    "before": a,
                    "after": b,
                    "delta": b - a,
                })

    freq_changes = [abs(x["delta_percent"]) for x in freq]
    thermal_changes = [abs(x["delta"]) for x in thermal]

    payload = {
        "schema": "RMR-PHYSICAL-TELEMETRY-DELTA-V1",
        "claim_allowed": False,
        "frequency_rows": freq,
        "thermal_rows": thermal,
        "max_abs_frequency_delta_percent": max(freq_changes) if freq_changes else None,
        "max_abs_thermal_delta_raw": max(thermal_changes) if thermal_changes else None,
        "interpretation_state": (
            "OBSERVED"
            if freq or thermal
            else "TOKEN_VAZIO_PLATFORM_TELEMETRY_UNAVAILABLE"
        ),
        "boundary": [
            "TELEMETRY_DELTA!=CAUSAL_THROTTLING_PROOF",
            "CI_TELEMETRY!=PHYSICAL_DEVICE_RECEIPT",
        ],
    }

    Path(args.out).write_text(
        json.dumps(payload, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    print("RMR_PHYSICAL_TELEMETRY_DELTA=PASS")
    print(f"frequency_rows={len(freq)} thermal_rows={len(thermal)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
