#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under ../LICENSE_RMR.

"""RMR source/build/binary topology auditor.

SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM.
The source scan is lexical and intentionally labels uncertain findings as
candidates. Linked symbols/ELF metadata come from nm/readelf when available.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

SOURCE_EXTENSIONS = {".c", ".h", ".cc", ".cpp", ".s", ".S", ".asm", ".py", ".rs", ".java", ".sh"}
C_SYMBOL_EXTENSIONS = {".c", ".cc", ".cpp"}
RESERVED_SYMBOL_NAMES = {"if", "for", "while", "switch", "return", "sizeof"}
SURFACE_BITS = {
    "PREPROCESSOR": 0,
    "COMPILER": 1,
    "LINKER": 2,
    "BINARY": 3,
    "SYMBOL": 4,
    "POINTER": 5,
    "LOOP": 6,
    "IO": 7,
    "COMMENT": 8,
    "WARNING": 9,
    "MODULE": 10,
    "OVERLAP": 11,
    "CONDITION": 12,
    "COLOR": 13,
    "IOPS": 14,
}

FUNC_RE = re.compile(
    r"^\s*(?!static\b)(?:[A-Za-z_]\w*(?:\s+|\s*\*\s*))+"
    r"([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{",
    re.MULTILINE,
)
INCLUDE_RE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.MULTILINE)
POINTER_DECL_RE = re.compile(
    r"\b(?:void|char|short|int|long|float|double|size_t|u?int(?:8|16|32|64)_t|"
    r"[A-Za-z_]\w*_t|struct\s+[A-Za-z_]\w*)\s*\*"
)
LOOP_RE = re.compile(r"\b(?:for|while)\s*\(|\bdo\b")
CONDITION_RE = re.compile(r"\b(?:if|switch)\s*\(|\?[^:\n]+:")
COLOR_RE = re.compile(r"\bcolor\b|ANSI|\\x1b\[", re.IGNORECASE)
IOPS_RE = re.compile(r"\biops\b", re.IGNORECASE)
PREPROCESSOR_RE = re.compile(r"^\s*#\s*[A-Za-z_]+", re.MULTILINE)
WARNING_RE = re.compile(r"#\s*warning\b|#\s*pragma\b[^\n]*diagnostic|(?<!\w)-W[A-Za-z0-9_-]+")
VOID_RE = re.compile(r"\bvoid\b")
VOID_PTR_RE = re.compile(r"\bvoid\s*\*")
COMMENT_LINE_RE = re.compile(r"(^\s*//)|(^\s*/\*)|(^\s*\*)|(//)", re.MULTILINE)


def run_tool(argv: list[str], stdin: str | None = None) -> dict:
    exe = shutil.which(argv[0])
    if not exe:
        return {"state": "TOKEN_VAZIO_TOOL_MISSING", "argv": argv}
    proc = subprocess.run(
        [exe, *argv[1:]],
        input=stdin,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    return {
        "state": "PASS" if proc.returncode == 0 else "FAIL",
        "returncode": proc.returncode,
        "argv": argv,
        "stdout": proc.stdout,
        "stderr": proc.stderr,
    }


def source_files(root: Path) -> list[Path]:
    return sorted(
        p for p in root.rglob("*")
        if p.is_file() and p.suffix in SOURCE_EXTENSIONS and ".git" not in p.parts
    )


def module_name(root: Path, path: Path) -> str:
    rel = path.relative_to(root)
    return rel.parts[0] if len(rel.parts) > 1 else "."


def scan_source(root: Path) -> dict:
    totals = Counter()
    modules: dict[str, Counter] = defaultdict(Counter)
    includes = []
    symbol_files: dict[str, list[str]] = defaultdict(list)
    entrypoint_files: list[str] = []
    files_out = []

    for path in source_files(root):
        rel = path.relative_to(root).as_posix()
        text = path.read_text(encoding="utf-8", errors="replace")
        lines = text.splitlines()
        mod = module_name(root, path)

        metrics = {
            "lines": len(lines),
            "bytes": len(text.encode("utf-8", errors="replace")),
            "void_tokens": len(VOID_RE.findall(text)),
            "void_pointer_boundaries": len(VOID_PTR_RE.findall(text)),
            "loop_tokens": len(LOOP_RE.findall(text)),
            "condition_tokens": len(CONDITION_RE.findall(text)),
            "color_markers": len(COLOR_RE.findall(text)),
            "iops_markers": len(IOPS_RE.findall(text)),
            "pointer_decl_candidates": len(POINTER_DECL_RE.findall(text)),
            "pointer_member_access": text.count("->"),
            "preprocessor_directives": len(PREPROCESSOR_RE.findall(text)),
            "warning_markers": len(WARNING_RE.findall(text)),
            "comment_lines": len(COMMENT_LINE_RE.findall(text)),
        }

        for key, value in metrics.items():
            totals[key] += value
            modules[mod][key] += value

        for inc in INCLUDE_RE.findall(text):
            includes.append({"parent": rel, "child": inc})

        if path.suffix in C_SYMBOL_EXTENSIONS:
            for symbol in FUNC_RE.findall(text):
                if symbol in RESERVED_SYMBOL_NAMES:
                    continue
                if symbol == "main":
                    entrypoint_files.append(rel)
                    continue
                symbol_files[symbol].append(rel)

        files_out.append({"path": rel, "module": mod, **metrics})

    duplicates = {
        name: sorted(set(paths))
        for name, paths in sorted(symbol_files.items())
        if len(set(paths)) > 1
    }
    conditional_providers = {
        name: paths
        for name, paths in duplicates.items()
        if all(path.startswith("hwif/detect/") for path in paths)
    }
    overlaps = {
        name: paths
        for name, paths in duplicates.items()
        if name not in conditional_providers
    }

    totals["files"] = len(files_out)
    comment_ratio = (
        float(totals["comment_lines"]) / float(totals["lines"])
        if totals["lines"] else 0.0
    )

    return {
        "state": "PASS",
        "root": str(root),
        "totals": dict(totals),
        "comment_line_ratio": comment_ratio,
        "modules": {k: dict(v) for k, v in sorted(modules.items())},
        "include_edges": includes,
        "entrypoint_candidates": sorted(entrypoint_files),
        "conditional_provider_candidates": conditional_providers,
        "source_symbol_overlap_candidates": overlaps,
        "files": files_out,
        "boundary": {
            "source_symbol_candidate": "not_linked_symbol",
            "duplicate_candidate": "not_linker_collision",
            "entrypoint_candidate": "not_symbol_overlap",
            "conditional_provider": "compile_time_selected_not_linker_collision",
            "void_pointer": "generic_boundary_not_missing_semantics",
            "comment": "not_execution",
        },
    }


def compiler_snapshot(compiler: str) -> dict:
    version = run_tool([compiler, "--version"])
    macros = run_tool([compiler, "-dM", "-E", "-x", "c", "-"], stdin="")
    selected = {}
    macro_count = None

    if macros.get("state") == "PASS":
        lines = [line for line in macros["stdout"].splitlines() if line.startswith("#define ")]
        macro_count = len(lines)
        wanted = (
            "__STDC", "__GNUC", "__clang", "__SIZEOF_POINTER__",
            "__BYTE_ORDER__", "__ORDER_", "__ARM", "__aarch64__",
            "__x86_64__", "__riscv",
        )
        for line in lines:
            parts = line.split(maxsplit=2)
            if len(parts) >= 2 and parts[1].startswith(wanted):
                selected[parts[1]] = parts[2] if len(parts) == 3 else "1"

    return {
        "compiler": compiler,
        "version_state": version.get("state"),
        "version_first_line": version.get("stdout", "").splitlines()[:1],
        "predefined_macro_state": macros.get("state"),
        "predefined_macro_count": macro_count,
        "selected_macros": selected,
    }


def parse_elf_header(text: str) -> dict:
    fields = {}
    for line in text.splitlines():
        if ":" not in line:
            continue
        key, value = line.split(":", 1)
        key = key.strip()
        if key in {"Class", "Data", "Type", "Machine", "Entry point address"}:
            fields[key] = value.strip()
    return fields


def binary_snapshot(binary: Path) -> dict:
    if not binary.exists():
        return {"state": "FAIL", "error": "binary_not_found", "path": str(binary)}

    header = run_tool(["readelf", "-h", str(binary)])
    programs = run_tool(["readelf", "-l", "-W", str(binary)])
    sections = run_tool(["readelf", "-S", "-W", str(binary)])
    relocs = run_tool(["readelf", "-r", "-W", str(binary)])
    defined = run_tool(["nm", "-g", "--defined-only", str(binary)])
    undefined = run_tool(["nm", "-u", str(binary)])
    sizes = run_tool(["size", "-A", str(binary)])

    def nonempty_lines(result: dict) -> int | None:
        if result.get("state") != "PASS":
            return None
        return sum(1 for line in result.get("stdout", "").splitlines() if line.strip())

    gnu_stack_line = None
    gnu_stack_executable = None
    if programs.get("state") == "PASS":
        for line in programs.get("stdout", "").splitlines():
            if "GNU_STACK" in line:
                gnu_stack_line = line.strip()
                flags = [
                    token for token in line.split()
                    if token and set(token).issubset(set("RWE")) and any(ch in token for ch in "RWE")
                ]
                gnu_stack_executable = any("E" in token for token in flags)
                break

    return {
        "state": "PASS",
        "path": str(binary),
        "size_bytes": binary.stat().st_size,
        "elf_header_state": header.get("state"),
        "elf_header": parse_elf_header(header.get("stdout", "")),
        "program_header_tool_state": programs.get("state"),
        "gnu_stack_line": gnu_stack_line,
        "gnu_stack_executable": gnu_stack_executable,
        "section_tool_state": sections.get("state"),
        "section_lines": nonempty_lines(sections),
        "relocation_tool_state": relocs.get("state"),
        "relocation_lines": nonempty_lines(relocs),
        "defined_symbol_tool_state": defined.get("state"),
        "defined_global_symbol_lines": nonempty_lines(defined),
        "undefined_symbol_tool_state": undefined.get("state"),
        "undefined_symbol_lines": nonempty_lines(undefined),
        "size_tool_state": sizes.get("state"),
        "size_output": sizes.get("stdout", ""),
        "boundary": "ELF_tool_output_is_build_evidence_not_runtime_performance",
    }


def color_enabled(mode: str) -> bool:
    if mode == "always":
        return True
    if mode == "never":
        return False
    return sys.stdout.isatty()


def paint(text: str, code: str, enabled: bool) -> str:
    return f"\x1b[{code}m{text}\x1b[0m" if enabled else text


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default="rmr")
    parser.add_argument("--binary")
    parser.add_argument("--compiler", default=os.environ.get("CC", "cc"))
    parser.add_argument("--json-out")
    parser.add_argument("--color", choices=("auto", "always", "never"), default="auto")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    if not root.is_dir():
        print(f"error: root not found: {root}", file=sys.stderr)
        return 2

    source = scan_source(root)
    binary = binary_snapshot(Path(args.binary).resolve()) if args.binary else {
        "state": "TOKEN_VAZIO_NOT_REQUESTED"
    }
    report = {
        "schema": "RMR-HARDWARE-BUILD-TOPOLOGY-V1",
        "surface_bits": SURFACE_BITS,
        "surface_mask": sum(1 << bit for bit in SURFACE_BITS.values()),
        "source": source,
        "compiler": compiler_snapshot(args.compiler),
        "binary": binary,
        "boundaries": [
            "SOURCE!=BUILD!=EXECUTION!=EVIDENCE!=CLAIM",
            "LOGICAL_IO_OPS!=PHYSICAL_STORAGE_IOPS",
            "SOURCE_SYMBOL_CANDIDATE!=LINKED_SYMBOL",
            "COMMENT!=EXECUTION",
            "WARNING!=FAILURE",
            "COLOR!=STATE",
            "IO!=IOPS",
        ],
    }

    overlap_count = len(source["source_symbol_overlap_candidates"])
    enabled = color_enabled(args.color)
    if overlap_count:
        status = paint("AUDIT: REVIEW", "33", enabled)
    else:
        status = paint("AUDIT: PASS(source topology)", "32", enabled)

    print(status)
    print(
        f"files={source['totals'].get('files', 0)} "
        f"loops={source['totals'].get('loop_tokens', 0)} "
        f"conditions={source['totals'].get('condition_tokens', 0)} "
        f"void*={source['totals'].get('void_pointer_boundaries', 0)} "
        f"pointer_candidates={source['totals'].get('pointer_decl_candidates', 0)} "
        f"warning_markers={source['totals'].get('warning_markers', 0)} "
        f"symbol_overlap_candidates={overlap_count}"
    )
    print("logical_io_ops != physical_storage_iops")
    if binary.get("gnu_stack_executable") is not None:
        print(f"gnu_stack_executable={str(binary['gnu_stack_executable']).lower()}")
    for symbol, paths in sorted(source["conditional_provider_candidates"].items()):
        print(f"conditional_provider={symbol} paths={','.join(paths)}")
    for symbol, paths in sorted(source["source_symbol_overlap_candidates"].items()):
        print(f"overlap_candidate={symbol} paths={','.join(paths)}")

    if args.json_out:
        out = Path(args.json_out)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
        print(f"json={out}")

    if binary.get("gnu_stack_executable") is True:
        print("error: executable GNU_STACK detected", file=sys.stderr)
        return 4

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
