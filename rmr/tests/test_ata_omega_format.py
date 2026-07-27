#!/usr/bin/env python3

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

from __future__ import annotations

import importlib.util
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MODULE_PATH = ROOT / "rmr" / "tools" / "ata_decode.py"
SPEC = importlib.util.spec_from_file_location("ata_decode", MODULE_PATH)
assert SPEC and SPEC.loader
ATA = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(ATA)


def records() -> bytes:
    return b"".join(struct.pack("<QQQ", k, 0x1000 + k, ((k - 1) % 9) + 1) for k in range(1, 43))


def test_v1() -> None:
    sig = 0x1122334455667788
    blob = struct.pack("<4sIIIIQ", b"RFA\0", 1, 24, 42, 0, sig) + records()
    decoded = ATA.decode_ata_bytes(blob)
    assert decoded["format"] == "V1_EXTENDED"
    assert decoded["hw_sig"] == sig
    assert len(decoded["records"]) == 42
    assert decoded["records"][0] == (1, 0x1001, 1)


def test_legacy() -> None:
    sig = 0x8877665544332211
    blob = struct.pack("<4sQ", b"RFA\0", sig) + records()
    decoded = ATA.decode_ata_bytes(blob)
    assert decoded["format"] == "LEGACY_COMPACT"
    assert decoded["hw_sig"] == sig
    assert len(decoded["records"]) == 42


def test_rejects_truncation() -> None:
    blob = struct.pack("<4sIIIIQ", b"RFA\0", 1, 24, 42, 0, 1) + records()[:-1]
    try:
        ATA.decode_ata_bytes(blob)
    except ValueError:
        return
    raise AssertionError("V1 truncado foi aceito")


def test_rejects_old_offset_confusion() -> None:
    # O leitor antigo interpretava version=1 + record_size=24 como hw_sig.
    blob = struct.pack("<4sIIIIQ", b"RFA\0", 1, 24, 42, 0, 0xAABBCCDDEEFF0011) + records()
    decoded = ATA.decode_ata_bytes(blob)
    assert decoded["hw_sig"] == 0xAABBCCDDEEFF0011
    assert decoded["hw_sig"] != 0x0000001800000001


if __name__ == "__main__":
    test_v1()
    test_legacy()
    test_rejects_truncation()
    test_rejects_old_offset_confusion()
    print("OK: ATA OMEGA V1 + legacy")
