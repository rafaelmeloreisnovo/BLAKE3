#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

from __future__ import annotations

import importlib.util
import struct
from pathlib import Path

MODULE_PATH = Path(__file__).resolve().parents[1] / "tools" / "pai42_bridge.py"
spec = importlib.util.spec_from_file_location("pai42_bridge", MODULE_PATH)
assert spec and spec.loader
bridge = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bridge)


def records() -> bytes:
    return b"".join(struct.pack("<QQQ", i + 1, 1000 + i * 7, 1 + i % 9) for i in range(42))


def v1() -> bytes:
    return b"RFA\x00" + struct.pack("<IIIIQ", 1, 24, 42, 0, 0x123456789ABCDEF0) + records()


def legacy() -> bytes:
    return b"RFA\x00" + struct.pack("<Q", 0x123456789ABCDEF0) + records()


def main() -> int:
    report_v1 = bridge.build_geometry(bridge.decode_ata(v1()))
    report_legacy = bridge.build_geometry(bridge.decode_ata(legacy()))
    assert report_v1["source_format"] == "V1_EXTENDED"
    assert report_legacy["source_format"] == "LEGACY_COMPACT"
    assert report_v1["points"] == report_legacy["points"]
    assert report_v1["points"][0]["radius_q16"] == 0
    assert report_v1["points"][-1]["radius_q16"] == 65536
    try:
        bridge.decode_ata(v1()[:-1])
    except ValueError:
        pass
    else:
        raise AssertionError("truncated ATA stream accepted")
    print("OK: RMR PAI42 Python bridge")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
