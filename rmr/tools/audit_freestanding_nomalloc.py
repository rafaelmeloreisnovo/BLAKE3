#!/usr/bin/env python3
"""Auditoria estática de coerência para o perfil freestanding nomalloc."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LOWLEVEL_H = ROOT / "rmr" / "include" / "rmr_lowlevel.h"
FREESTANDING_C = ROOT / "rmr" / "core" / "lowlevel_freestanding.c"
PATHCUTTER_C = ROOT / "rmr" / "pathcutter" / "pathcutter.c"


def fail(msg: str) -> None:
    print(f"[FAIL] {msg}")


def ok(msg: str) -> None:
    print(f"[ OK ] {msg}")


def check_files_exist() -> int:
    errors = 0
    for path in (LOWLEVEL_H, FREESTANDING_C, PATHCUTTER_C):
        if not path.exists():
            fail(f"arquivo ausente: {path.relative_to(ROOT)}")
            errors += 1
    return errors


def check_nomalloc_guards(text: str) -> int:
    errors = 0
    required_tokens = (
        "#if defined(RMR_NO_LIBC) && defined(RMR_FREESTANDING_NOMALLOC)",
        "RMR_FREESTANDING_ARENA_SIZE",
        "rmr_ll_freestanding_available",
        "rmr_ll_freestanding_reset_allocator",
        "rmr_ll_calloc",
    )
    for token in required_tokens:
        if token not in text:
            fail(f"token obrigatório ausente em rmr_lowlevel.h: {token}")
            errors += 1
    return errors


def check_no_libc_calls_in_nomalloc(text: str) -> int:
    errors = 0
    block = re.search(
        r"#if defined\(RMR_NO_LIBC\) && defined\(RMR_FREESTANDING_NOMALLOC\)(.*?)#endif",
        text,
        flags=re.S,
    )
    if not block:
        fail("bloco nomalloc não encontrado")
        return 1
    segment = block.group(1)
    forbidden = ("malloc(", "calloc(", "realloc(", "free(", "memset(", "memcpy(")
    for token in forbidden:
        if token in segment:
            fail(f"uso proibido no bloco nomalloc: {token}")
            errors += 1
    return errors


def check_single_arena_definition(text: str) -> int:
    errors = 0
    if "uint8_t rmr_freestanding_arena" not in text:
        fail("definição da arena ausente em lowlevel_freestanding.c")
        errors += 1
    if "size_t rmr_freestanding_arena_head" not in text:
        fail("head da arena ausente em lowlevel_freestanding.c")
        errors += 1
    return errors


def check_pathcutter_memset(text: str) -> int:
    errors = 0
    if "rmr_memset(" not in text:
        fail("pathcutter não usa rmr_memset")
        errors += 1
    if "memset(" in text and "rmr_memset(" not in text:
        fail("pathcutter contém memset direto")
        errors += 1
    return errors


def main() -> int:
    errors = 0
    errors += check_files_exist()
    if errors:
        return 2

    lowlevel = LOWLEVEL_H.read_text(encoding="utf-8")
    freestanding = FREESTANDING_C.read_text(encoding="utf-8")
    pathcutter = PATHCUTTER_C.read_text(encoding="utf-8")

    errors += check_nomalloc_guards(lowlevel)
    errors += check_no_libc_calls_in_nomalloc(lowlevel)
    errors += check_single_arena_definition(freestanding)
    errors += check_pathcutter_memset(pathcutter)

    if errors:
        fail(f"auditoria freestanding reprovada com {errors} problema(s)")
        return 1
    ok("auditoria freestanding nomalloc aprovada")
    return 0


if __name__ == "__main__":
    sys.exit(main())
