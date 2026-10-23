#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
"""Read-only byte census of tracked RMR files. A zero count is not unknown.

Records SHA-256 of exact working-tree bytes AND a Git HEAD blob identity.
Filenames, header prefix, comments, void, warnings, ASCII, timestamp literals
are evidence dimensions, never presumed linker-optimization mechanisms.
This is a hosted audit tool, not part of a freestanding core.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCAN_EXT = {b".c", b".h", b".s", b".S", b".asm", b".rs", b".java",
            b".sh", b".mk", b".ld", b".yml", b".yaml", b".py"}
DATE = re.compile(rb"\b(?:19|20)\d\d[-/]\d\d[-/]\d\d(?:[ T]\d\d:\d\d(?::\d\d)?)?")
VOID = re.compile(rb"\bvoid\b")
WARN = re.compile(rb"-W(?:error|shadow|all|extra|pedantic|conversion|format(?:=2)?)\b")
COMMENT = re.compile(rb"(?://|/\*|#)[^\r\n]*")


def sha256(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()


def git(*argv: str) -> bytes:
    return subprocess.check_output(["git", "-C", str(ROOT), *argv])


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True, help="new output JSONL outside the checkout")
    ns = ap.parse_args()
    out = Path(ns.out).resolve()
    if out.is_relative_to(ROOT):
        raise RuntimeError("refusing output inside the source checkout")
    head = git("rev-parse", "HEAD").decode().strip()
    records = []
    total = 0
    non_ascii_files = 0
    source_changed = 0
    for item in git("ls-tree", "-r", "-z", "HEAD", "--", "rmr").split(b"\0"):
        if not item:
            continue
        header, raw_path = item.split(b"\t", 1)
        fields = header.decode("ascii").split(" ")
        mode, kind, git_oid = fields
        if kind != "blob":
            continue
        # git's -z mode is necessary: paths can contain whitespace/newlines.
        path = os.fsdecode(raw_path)
        src = ROOT / path
        data = src.read_bytes()
        total += 1
        is_source = os.fsencode(Path(path).suffix) in SCAN_EXT
        non_ascii = sum(b >= 128 for b in data)
        non_ascii_files += int(non_ascii > 0)
        # Blob hash uses the *exact* bytes, no clean/smudge filters.
        # Do not trust HEAD name or index presence as bytes evidence alone.
        local_oid = git("hash-object", "--no-filters", "--", path).decode().strip()
        changed = local_oid != git_oid
        source_changed += int(changed)
        entry = {
            "path": path,
            "path_utf8_hex": raw_path.hex(),
            "name_ascii": all(c < 128 for c in raw_path),
            "git_mode": mode,
            "head_blob_oid": git_oid,
            "working_blob_oid": local_oid,
            "working_bytes_equal_head": not changed,
            "bytes": len(data),
            "sha256": sha256(data),
            "header_prefix_256_sha256": sha256(data[:256]),
            "high_bit_bytes": non_ascii,
            "nul_bytes": data.count(b"\0"),
            "crlf_count": data.count(b"\r\n"),
            "lf_count": data.count(b"\n"),
            "void_lexemes": len(VOID.findall(data)) if is_source else 0,
            "warning_flag_lexemes": len(WARN.findall(data)) if is_source else 0,
            "comment_markers_lexical": len(COMMENT.findall(data)) if is_source else 0,
            "date_time_lexemes": len(DATE.findall(data)),
            "semantics": "LEXICAL_ONLY_NOT_EXECUTION_PROOF",
        }
        records.append((raw_path, entry))
    records.sort(key=lambda pair: pair[0])
    out.parent.mkdir(parents=True, exist_ok=True)
    # Stable key order and compact form, ASCII escapes explicit in UTF-8 JSONL.
    with out.open("wb") as f:
        for _, entry in records:
            f.write((json.dumps(entry, sort_keys=True, ensure_ascii=True,
                                separators=(",", ":")) + "\n").encode("ascii"))
    print("RMR_BYTE_CENSUS_HEAD=" + head)
    print("RMR_BYTE_CENSUS_FILES=" + str(total))
    print("RMR_BYTE_CENSUS_NON_ASCII_FILES=" + str(non_ascii_files))
    print("RMR_BYTE_CENSUS_CHANGED_FROM_HEAD=" + str(source_changed))
    print("RMR_BYTE_CENSUS_SHA256=" + sha256(out.read_bytes()))
    print("RMR_BYTE_CENSUS_SCOPE=SOURCE_BYTES_NOT_LINKER_CAUSALITY")
    return 1 if source_changed else 0


if __name__ == "__main__":
    sys.exit(main())
