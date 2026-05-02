#!/usr/bin/env python3
"""Auditoria estática para o módulo rmr/pathcutter."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MODULE_DIR = ROOT / "rmr" / "pathcutter"
CORE_DIR = ROOT / "rmr" / "core"

FORBIDDEN_INCLUDES = {
    "openssl/",
    "curl/",
    "python",
    "sqlite3",
    "zmq",
}

HEAP_CALLS = ("malloc(", "calloc(", "realloc(", "free(")
ALLOWED_EXPORTS = {"rmr_pc_die", "rmr_pc_mkdir_p", "rmr_pc_xmalloc", "rmr_pc_xfree"}
ALLOWED_CALL_PREFIXES = ("rmr_ll_", "perror", "fprintf", "mem", "str", "exit", "mkdir")


def module_files() -> list[Path]:
    return sorted(MODULE_DIR.glob("*.[ch]"))


def fail(msg: str) -> None:
    print(f"[FAIL] {msg}")


def ok(msg: str) -> None:
    print(f"[ OK ] {msg}")


def check_includes(files: list[Path]) -> int:
    errors = 0
    inc_re = re.compile(r'^\s*#\s*include\s*[<\"]([^>\"]+)[>\"]')
    for file in files:
        for n, line in enumerate(file.read_text(encoding="utf-8").splitlines(), start=1):
            m = inc_re.match(line)
            if not m:
                continue
            inc = m.group(1).lower()
            if any(token in inc for token in FORBIDDEN_INCLUDES):
                fail(f"include proibido em {file.relative_to(ROOT)}:{n}: {m.group(1)}")
                errors += 1
    return errors


def check_heap_calls(files: list[Path]) -> int:
    errors = 0
    for file in files:
        lines = file.read_text(encoding="utf-8").splitlines()
        for n, line in enumerate(lines, start=1):
            if "for (" in line or "while (" in line:
                window = "\n".join(lines[n - 1 : min(n + 6, len(lines))])
                if any(call in window for call in HEAP_CALLS):
                    fail(f"heap call em hot loop {file.relative_to(ROOT)}:{n}")
                    errors += 1
    return errors


def check_contract_usage() -> int:
    errors = 0
    text = (CORE_DIR / "util.c").read_text(encoding="utf-8")
    used = set(re.findall(r"rmr_pc_[A-Za-z0-9_]+", text))
    unknown = sorted(sym for sym in used if sym not in ALLOWED_EXPORTS)
    missing = sorted(sym for sym in ALLOWED_EXPORTS if sym not in used)
    for sym in unknown:
        fail(f"simbolo fora do contrato usado em rmr/core/util.c: {sym}")
        errors += 1
    for sym in missing:
        fail(f"simbolo do contrato nao consumido em rmr/core/util.c: {sym}")
        errors += 1
    return errors


def check_external_calls() -> int:
    errors = 0
    text = (MODULE_DIR / "pathcutter.c").read_text(encoding="utf-8")
    for call in re.findall(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(", text):
        if call in {"if", "for", "while", "return", "sizeof"}:
            continue
        if call.startswith("rmr_pc_"):
            continue
        if not call.startswith(ALLOWED_CALL_PREFIXES):
            fail(f"chamada externa fora do contrato no modulo: {call}()")
            errors += 1
    return errors


def main() -> int:
    files = module_files()
    if not files:
        fail("modulo rmr/pathcutter nao encontrado")
        return 2

    ok(f"auditoria em {len(files)} arquivos de {MODULE_DIR.relative_to(ROOT)}")
    errors = 0
    errors += check_includes(files)
    errors += check_heap_calls(files)
    errors += check_contract_usage()
    errors += check_external_calls()

    if errors:
        fail(f"auditoria reprovada com {errors} problema(s)")
        return 1

    ok("auditoria aprovada")
    return 0


if __name__ == "__main__":
    sys.exit(main())
