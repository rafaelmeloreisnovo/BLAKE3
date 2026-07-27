#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

"""Decode ATA OMEGA and emit a deterministic 42-point PAI geometry report."""

from __future__ import annotations

import argparse
import json
import math
import struct
from pathlib import Path
from typing import Any

MAGIC = b"RFA\x00"
COUNT = 42
RECORD_SIZE = 24
Q16_ONE = 1 << 16
V1_HEADER_SIZE = 28
LEGACY_HEADER_SIZE = 12


def _u32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def _u64(data: bytes, offset: int) -> int:
    return struct.unpack_from("<Q", data, offset)[0]


def decode_ata(data: bytes) -> dict[str, Any]:
    if len(data) < LEGACY_HEADER_SIZE or data[:4] != MAGIC:
        raise ValueError("ATA OMEGA magic/length invalid")

    is_v1 = (
        len(data) >= V1_HEADER_SIZE
        and _u32(data, 4) == 1
        and _u32(data, 8) == RECORD_SIZE
        and 1 <= _u32(data, 12) <= COUNT
        and _u32(data, 16) == 0
    )
    if is_v1:
        fmt = "V1_EXTENDED"
        count = _u32(data, 12)
        hw_sig64 = _u64(data, 20)
        offset = V1_HEADER_SIZE
    else:
        fmt = "LEGACY_COMPACT"
        count = COUNT
        hw_sig64 = _u64(data, 4)
        offset = LEGACY_HEADER_SIZE

    expected = offset + count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"ATA OMEGA length mismatch: expected {expected}, got {len(data)}")

    records: list[dict[str, int]] = []
    for index in range(count):
        k, cyc, par = struct.unpack_from("<QQQ", data, offset + index * RECORD_SIZE)
        if not 1 <= k <= COUNT or not 1 <= par <= 9:
            raise ValueError(f"invalid ATA record at index {index}")
        if is_v1 and k != index + 1:
            raise ValueError(f"non-canonical ATA sequence at index {index}")
        records.append({"k": k, "cycle": cyc, "par": par})

    if count != COUNT:
        raise ValueError("PAI42 requires exactly 42 ATA records")
    return {"format": fmt, "hw_sig64": hw_sig64, "records": records}


def build_geometry(decoded: dict[str, Any]) -> dict[str, Any]:
    cycles = [int(record["cycle"]) for record in decoded["records"]]
    minimum = min(cycles)
    maximum = max(cycles)
    span = maximum - minimum
    radii = [Q16_ONE // 2] * COUNT if span == 0 else [
        ((cycle - minimum) * Q16_ONE) // span for cycle in cycles
    ]
    mean_radius = sum(radii) // COUNT
    variance_q16 = min(
        Q16_ONE,
        sum((radius - mean_radius) ** 2 for radius in radii) // (COUNT * Q16_ONE),
    )
    opposite_error = sum(abs(radii[i] - radii[i + COUNT // 2]) for i in range(COUNT // 2))
    symmetry_q16 = Q16_ONE - min(Q16_ONE, opposite_error // (COUNT // 2))
    stability_q16 = Q16_ONE - variance_q16
    score_q16 = (55 * symmetry_q16 + 45 * stability_q16) // 100

    points = []
    for index, radius in enumerate(radii):
        angle = 2.0 * math.pi * index / COUNT
        cos_q16 = round(math.cos(angle) * Q16_ONE)
        sin_q16 = round(math.sin(angle) * Q16_ONE)
        points.append({
            "index": index,
            "k": decoded["records"][index]["k"],
            "cycle": cycles[index],
            "par": decoded["records"][index]["par"],
            "radius_q16": radius,
            "x_q16": (cos_q16 * radius) >> 16,
            "y_q16": (sin_q16 * radius) >> 16,
        })

    return {
        "contract": "RMR_PAI42_V1",
        "source_format": decoded["format"],
        "hw_sig64_hex": f"0x{decoded['hw_sig64']:016x}",
        "count": COUNT,
        "metrics": {
            "min_cycle": minimum,
            "max_cycle": maximum,
            "mean_radius_q16": mean_radius,
            "variance_q16": variance_q16,
            "symmetry_q16": symmetry_q16,
            "stability_q16": stability_q16,
            "prototype_score_q16": score_q16,
        },
        "points": points,
        "claim_state": {
            "geometric_projection": "VERIFIED_BY_CODE",
            "semantic_classification": "TOKEN_VAZIO",
            "hardware_uniqueness": "NOT_CLAIMED",
            "cryptographic_binding": "TOKEN_VAZIO",
        },
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="ATA_OMEGA.bin")
    parser.add_argument("--out", type=Path, help="write canonical JSON to this path")
    args = parser.parse_args()

    report = build_geometry(decode_ata(args.input.read_bytes()))
    payload = json.dumps(report, ensure_ascii=False, sort_keys=True, separators=(",", ":")) + "\n"
    if args.out:
        args.out.write_text(payload, encoding="utf-8")
    else:
        print(payload, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
