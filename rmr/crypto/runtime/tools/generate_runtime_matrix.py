#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

import argparse
import json
import platform
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
REGISTRY = ROOT / "rmr/crypto/runtime/registry.json"

def native_profile(architectures):
    machine = platform.machine().lower()
    for item in architectures:
        aliases = {a.lower() for a in item.get("aliases", [])}
        if machine == item["id"].lower() or machine in aliases:
            return item["id"]
    return None

def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--native-kat-pass", action="store_true")
    parser.add_argument("--out", required=True)
    args = parser.parse_args()

    data = json.loads(REGISTRY.read_text(encoding="utf-8"))
    architectures = data["architectures"]
    algorithms = data["algorithms"]
    native = native_profile(architectures)

    rows = []
    for arch in architectures:
        for alg in algorithms:
            row = {
                "architecture": arch["id"],
                "algorithm": alg["id"],
                "provider": alg["provider"],
                "claim_allowed": False,
                "source_state": (
                    "BLAKE3_BACKEND_REGISTERED"
                    if alg["id"] == "blake3"
                    else "RMR_PROVIDER_API_REGISTERED"
                ),
                "execution_state": "TOKEN_VAZIO_PROVIDER_TOOLCHAIN",
            }

            if args.native_kat_pass and native == arch["id"]:
                row["execution_state"] = "KAT_PASS_NATIVE"
            elif alg["id"] == "blake3":
                row["execution_state"] = "TOKEN_VAZIO_NOT_EXECUTED_ON_THIS_ARCH"

            if alg.get("security_use") == "compatibility_only":
                row["security_use"] = "compatibility_only"

            rows.append(row)

    out = {
        "schema": "RMR-CRYPTO-RUNTIME-CARTESIAN-V1",
        "claim_allowed": False,
        "native_machine": platform.machine(),
        "native_profile": native,
        "native_kat_pass": bool(args.native_kat_pass),
        "architectures": len(architectures),
        "algorithms": len(algorithms),
        "cells": len(rows),
        "rows": rows,
        "boundary": [
            "KAT_PASS_NATIVE!=CROSS_ARCH_PASS",
            "BLAKE3_BACKEND_REGISTERED!=EXECUTED",
            "PROVIDER_API_REGISTERED!=PROVIDER_AVAILABLE_ON_TARGET"
        ]
    }

    target = Path(args.out)
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(json.dumps(out, indent=2, sort_keys=True) + "\n", encoding="utf-8")

    if len(rows) != len(architectures) * len(algorithms):
        return 1

    print(
        f"RMR_CRYPTO_CARTESIAN=PASS cells={len(rows)} "
        f"native_profile={native or 'TOKEN_VAZIO'}"
    )
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
