#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

import csv
import json
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
MODEL = ROOT / "rmr/benchmark_framework/simperf/simperf_model.py"
PROFILES = ROOT / "rmr/benchmark_framework/simperf/hardware_profiles.json"

def write_csv(path: Path, values):
    with path.open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["algorithm", "size_bytes", "ns_per_op"])
        for row in values:
            w.writerow(row)

def main() -> int:
    with tempfile.TemporaryDirectory() as td:
        td = Path(td)
        official = td / "official.csv"
        candidate = td / "candidate.csv"
        out = td / "out.json"

        write_csv(official, [
            ("blake3", 64, 100.0),
            ("blake3", 1024, 500.0),
            ("blake3", 1048576, 300000.0),
        ])
        write_csv(candidate, [
            ("blake3", 64, 90.0),
            ("blake3", 1024, 400.0),
            ("blake3", 1048576, 250000.0),
        ])

        subprocess.run([
            sys.executable, str(MODEL),
            "--profiles", str(PROFILES),
            "--official", str(official),
            "--candidate", str(candidate),
            "--reference-profile", "x86_64-avx2-desktop",
            "--sizes", "64,1024",
            "--out", str(out),
        ], check=True)

        data = json.loads(out.read_text(encoding="utf-8"))
        assert data["claim_allowed"] is False
        assert data["not_cycle_accurate"] is True
        assert len(data["rows"]) == 10  # 5 hardware profiles x 2 sizes.
        for row in data["rows"]:
            assert row["model_state"] == "ANALOGY_ESTIMATE"
            assert row["official_est_ns_per_op"] > 0
            assert row["official_est_ns_low"] <= row["official_est_ns_per_op"]
            assert row["official_est_ns_high"] >= row["official_est_ns_per_op"]
            assert row["candidate_est_ns_per_op"] > 0

    print("RMR_SIMPERF_SELFTEST=PASS")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
