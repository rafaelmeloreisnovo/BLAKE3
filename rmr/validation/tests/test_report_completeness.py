#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

from __future__ import annotations

import csv
import json
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
FULL = ROOT / "rmr/validation/generate_full_validation_report.py"
COMP = ROOT / "rmr/upstream_validation/generate_v3_report.py"


def write_json(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload) + "\n", encoding="utf-8")


def run_report(script: Path, artifacts: Path, out: Path) -> dict:
    jout = out / "report.json"
    mout = out / "report.md"
    out.mkdir(parents=True, exist_ok=True)
    subprocess.run(
        [
            sys.executable,
            str(script),
            "--artifacts",
            str(artifacts),
            "--json-out",
            str(jout),
            "--md-out",
            str(mout),
        ],
        check=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    return json.loads(jout.read_text(encoding="utf-8"))


def make_full(root: Path) -> None:
    (root / "san/transcript.txt").parent.mkdir(parents=True, exist_ok=True)
    (root / "san/transcript.txt").write_text("PASS\n", encoding="utf-8")
    write_json(root / "core.json", {
        "schema": "RMR-BLAKE3-UPSTREAM-COMPARE-V2",
        "analysis": [{"classification": "PARITY_OR_NOISE_NOT_SEPARATED"}],
    })
    write_json(root / "ablation.json", {"schema": "RMR-C-ABLATION-V3"})
    write_json(root / "backend.json", {"schema": "RMR-BLAKE3-BACKEND-MATRIX-V3"})
    write_json(root / "rustlib.json", {"schema": "RMR-RUST-LIBRARY-V3"})
    write_json(root / "rustcli.json", {"schema": "RMR-RUST-CLI-V3"})
    write_json(root / "binary.json", {
        "schema": "RMR-BINARY-SURFACE-AUDIT-V1",
        "public_api_contract": "PASS",
        "fork_gnu_stack_gate": "PASS",
        "strict_warning_state": "PASS",
    })
    with (root / "status.csv").open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["profile", "state", "target"])
        w.writerow(["x86_64", "PASS", "x86_64-none-elf"])


def make_comp(root: Path) -> None:
    write_json(root / "source.json", {
        "schema": "RMR-UPSTREAM-SOURCE-DELTA-V3",
        "changed": [{"path": "c/blake3.h"}],
        "performance_relevant_changed": ["c/blake3.h"],
    })
    (root / "quality.txt").write_text("RMR_C_QUALITY_V3=PASS\n", encoding="utf-8")
    write_json(root / "core.json", {
        "schema": "RMR-BLAKE3-UPSTREAM-COMPARE-V2",
        "analysis": [{"classification": "PARITY_OR_NOISE_NOT_SEPARATED"}],
    })
    write_json(root / "ablation.json", {"schema": "RMR-C-ABLATION-V3"})
    write_json(root / "backend.json", {
        "schema": "RMR-BLAKE3-BACKEND-MATRIX-V3",
        "rows": [],
    })
    write_json(root / "tbb.json", {"schema": "RMR-TBB-V3", "rows": []})
    write_json(root / "abi.json", {
        "schema": "RMR-BLAKE3-ABI-ELF-V3",
        "gnu_stack": {"fork": "PASS_NON_EXECUTABLE"},
        "public_abi_probe_equal": True,
        "symbols_only_fork": [],
    })
    write_json(root / "rustlib.json", {
        "schema": "RMR-RUST-LIBRARY-V3",
        "rows": [],
    })
    write_json(root / "rustcli.json", {
        "schema": "RMR-RUST-CLI-V3",
        "rows": [],
    })
    write_json(root / "cross.json", {
        "schema": "RMR-CROSS-ARCH-V3",
        "targets": ["x86_64", "armv7"],
    })
    (root / "integration.txt").write_text(
        "RMR_INTEGRATION_V3=PASS\n", encoding="utf-8"
    )


def main() -> int:
    with tempfile.TemporaryDirectory() as td:
        base = Path(td)

        full = base / "full"
        make_full(full)
        complete = run_report(FULL, full, base / "full-out")
        assert complete["execution_complete"] is True
        assert complete["validation_state"] == "COMPLETE"
        assert complete["missing_required_axes"] == []

        (full / "rustcli.json").unlink()
        partial = run_report(FULL, full, base / "full-partial-out")
        assert partial["execution_complete"] is False
        assert partial["validation_state"] == "PARTIAL"
        assert "B3SUM" in partial["missing_required_axes"]

        comp = base / "comp"
        comp.mkdir()
        make_comp(comp)
        c_complete = run_report(COMP, comp, base / "comp-out")
        assert c_complete["execution_complete"] is True
        assert c_complete["validation_state"] == "COMPLETE"

        (comp / "integration.txt").unlink()
        c_partial = run_report(COMP, comp, base / "comp-partial-out")
        assert c_partial["execution_complete"] is False
        assert c_partial["validation_state"] == "PARTIAL"
        assert "RMR-INTEGRATION" in c_partial["missing_required_axes"]

    print("RMR_VALIDATION_COMPLETENESS_SELFTEST=PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
