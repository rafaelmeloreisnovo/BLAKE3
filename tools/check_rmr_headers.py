#!/usr/bin/env python3

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

import argparse
import subprocess
import sys
from pathlib import Path
from typing import Dict, Iterable, List, Sequence

TARGET_EXTENSIONS = {".c", ".h", ".S", ".sh", ".py", ".md"}
TARGET_ROOTS = ("rmr", "tools")
VIOLATION_KEYS = (
    "missing_license_header",
    "missing_shebang",
    "changes_outside_rmr_not_whitelisted",
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Valida cabeçalho LICENSE_RMR e shebang em rmr/ e tools/, "
            "e detecta mudanças fora de rmr/."
        ),
        epilog=(
            "Exemplos:\n"
            "  python3 tools/check_rmr_headers.py\n"
            "  python3 tools/check_rmr_headers.py --header-window 20 "
            "--allow-outside DOCUMENTACAO.md --allow-outside rmr/PROVENIENCE.md"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--header-window",
        type=int,
        default=20,
        help="Número de linhas iniciais para buscar LICENSE_RMR (entre 10 e 20).",
    )
    parser.add_argument(
        "--allow-outside",
        action="append",
        default=[],
        metavar="PATH",
        help=(
            "Caminho relativo ao repositório permitido fora de rmr/ "
            "na checagem de escopo (pode repetir)."
        ),
    )

    args = parser.parse_args()
    if args.header_window < 10 or args.header_window > 20:
        parser.error("--header-window deve estar entre 10 e 20")
    return args


def iter_target_files(repo_root: Path) -> Iterable[Path]:
    for root_name in TARGET_ROOTS:
        root = repo_root / root_name
        if not root.exists():
            continue
        for path in root.rglob("*"):
            if path.is_file() and path.suffix in TARGET_EXTENSIONS:
                yield path


def read_first_lines(path: Path, max_lines: int) -> List[str]:
    lines: List[str] = []
    with path.open("r", encoding="utf-8", errors="replace") as handle:
        for _ in range(max_lines):
            line = handle.readline()
            if line == "":
                break
            lines.append(line)
    return lines


def has_license_header(lines: Sequence[str]) -> bool:
    return any("LICENSE_RMR" in line for line in lines)


def has_shebang_line(lines: Sequence[str]) -> bool:
    return bool(lines) and lines[0].startswith("#!")


def get_changed_paths_outside_rmr(repo_root: Path) -> List[str]:
    cmd = ["git", "diff", "--name-only", "--", ".", ":(exclude)rmr/**"]
    completed = subprocess.run(
        cmd,
        cwd=repo_root,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    if completed.returncode != 0:
        message = completed.stderr.strip() or "git diff falhou"
        raise RuntimeError(message)

    paths = [line.strip() for line in completed.stdout.splitlines() if line.strip()]
    return paths


def report_violations(violations: Dict[str, List[str]]) -> None:
    print("Relatório de violações:")
    for key in VIOLATION_KEYS:
        items = sorted(set(violations[key]))
        print(f"{key}:")
        if not items:
            print("  - none")
            continue
        for item in items:
            print(f"  - {item}")


def main() -> int:
    args = parse_args()
    repo_root = Path(__file__).resolve().parent.parent
    allowed_outside = {p.strip() for p in args.allow_outside if p and p.strip()}

    violations: Dict[str, List[str]] = {key: [] for key in VIOLATION_KEYS}

    for target in iter_target_files(repo_root):
        rel = target.relative_to(repo_root).as_posix()
        first_lines = read_first_lines(target, args.header_window)

        if not has_license_header(first_lines):
            violations["missing_license_header"].append(rel)

        if target.suffix in {".sh", ".py"} and not has_shebang_line(first_lines):
            violations["missing_shebang"].append(rel)

    changed_outside = get_changed_paths_outside_rmr(repo_root)
    for path in changed_outside:
        normalized = Path(path).as_posix()
        if normalized not in allowed_outside:
            violations["changes_outside_rmr_not_whitelisted"].append(normalized)

    report_violations(violations)

    has_any_violation = any(violations[key] for key in VIOLATION_KEYS)
    return 1 if has_any_violation else 0


if __name__ == "__main__":
    sys.exit(main())
