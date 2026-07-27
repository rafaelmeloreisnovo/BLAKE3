#!/usr/bin/env python3

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

from __future__ import annotations

import argparse
import struct
from pathlib import Path
from typing import Any

MAGIC = b"RFA\0"
VERSION = 1
RECORD_SIZE = 24
MAX_RECORDS = 42
G = "0123456789ABCDEFGHIJ"


def v20(n: int) -> str:
    if n == 0:
        return "0"
    out: list[str] = []
    while n > 0:
        out.append(G[n % 20])
        n //= 20
    return "".join(reversed(out))


def _decode_records(data: bytes, count: int) -> list[tuple[int, int, int]]:
    expected = count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"payload invalido: esperado={expected} obtido={len(data)}")
    records: list[tuple[int, int, int]] = []
    for index in range(count):
        k, cyc, par = struct.unpack_from("<QQQ", data, index * RECORD_SIZE)
        if not (1 <= k <= MAX_RECORDS and 1 <= par <= 9):
            raise ValueError(f"registro invalido no indice {index}: k={k} par={par}")
        records.append((k, cyc, par))
    return records


def decode_ata_bytes(blob: bytes) -> dict[str, Any]:
    if len(blob) < 12 or blob[:4] != MAGIC:
        raise ValueError("magic ou tamanho inicial invalido")

    # V1: magic + version + record_size + count + reserved + hw_sig.
    if len(blob) >= 28:
        version, record_size, count, reserved = struct.unpack_from("<IIII", blob, 4)
        if (
            version == VERSION
            and record_size == RECORD_SIZE
            and 1 <= count <= MAX_RECORDS
            and reserved == 0
        ):
            hw_sig = struct.unpack_from("<Q", blob, 20)[0]
            records = _decode_records(blob[28:], count)
            for expected_k, (k, _, _) in enumerate(records, start=1):
                if k != expected_k:
                    raise ValueError(f"sequencia V1 invalida: esperado k={expected_k}, obtido={k}")
            return {"format": "V1_EXTENDED", "hw_sig": hw_sig, "records": records}

    # Formato histórico dos objetos binários: magic + hw_sig + registros.
    payload = blob[12:]
    if not payload or len(payload) % RECORD_SIZE != 0:
        raise ValueError("payload legacy truncado ou vazio")
    count = len(payload) // RECORD_SIZE
    if count > MAX_RECORDS:
        raise ValueError(f"quantidade legacy excede limite: {count}")
    hw_sig = struct.unpack_from("<Q", blob, 4)[0]
    records = _decode_records(payload, count)
    return {"format": "LEGACY_COMPACT", "hw_sig": hw_sig, "records": records}


def main() -> int:
    parser = argparse.ArgumentParser(description="Decodifica ATA_OMEGA.bin V1 ou legacy")
    parser.add_argument("path", nargs="?", default="ATA_OMEGA.bin")
    args = parser.parse_args()

    decoded = decode_ata_bytes(Path(args.path).read_bytes())
    records = decoded["records"]
    sig = decoded["hw_sig"]
    print(
        f"[OK] format={decoded['format']} magic=RFA\\0 "
        f"hw_sig=0x{sig:016x} hw_sig_v20={v20(sig)} recs={len(records)}"
    )
    for index, (k, cyc, par) in enumerate(records, start=1):
        print(f"[{index:02d}] k={k:02d} cyc=0x{cyc:016x} cyc_v20={v20(cyc)} p={par}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
