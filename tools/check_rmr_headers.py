#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
from pathlib import Path
import argparse
import re
import sys

ROOTS = (Path('rmr'), Path('tools'))
CHECK_EXTS = {
    '.c', '.h', '.S', '.s', '.sh', '.bash', '.py', '.rb', '.pl',
    '.md', '.txt', '.yaml', '.yml', '.toml', '.json', '.rs', '.ld', '.inc', '.mk'
}
ALLOWLIST = {
    Path('rmr/LICENSE_RMR'),
}

CANONICAL_AUTHOR = 'Rafael Melo Reis'
CANONICAL_YEARS = '2024–2026'
CANONICAL_LICENSE = 'Licensed under LICENSE_RMR.'

C_STYLE_EXTS = {'.c', '.h', '.S', '.s', '.ld', '.inc'}
HASH_STYLE_EXTS = {'.sh', '.bash', '.py', '.rb', '.pl', '.mk'}
HTML_STYLE_EXTS = {'.md', '.txt', '.yaml', '.yml', '.toml', '.json'}

LEGACY_LICENSE_RE = re.compile(r'License:\s*RMR Module License\s*\(see\s*(?:rmr/)?LICENSE_RMR\)', re.IGNORECASE)
CANONICAL_COPY_RE = re.compile(
    rf'Copyright\s*\(c\)\s*{re.escape(CANONICAL_YEARS)}\s+{re.escape(CANONICAL_AUTHOR)}'
)


def should_check(path: Path) -> bool:
    if path in ALLOWLIST or path.is_dir():
        return False
    if path.name == 'Makefile':
        return True
    return path.suffix in CHECK_EXTS


def extract_header_lines(path: Path, lines: list[str]) -> list[str]:
    if path.name == 'Makefile' or path.suffix in HASH_STYLE_EXTS:
        start = 1 if lines and lines[0].startswith('#!') else 0
        return lines[start:start + 8]
    return lines[:8]


def has_any_rmr_header(header_text: str) -> bool:
    return ('LICENSE_RMR' in header_text) or bool(LEGACY_LICENSE_RE.search(header_text))


def has_canonical_header(header_text: str) -> bool:
    return bool(CANONICAL_COPY_RE.search(header_text)) and (CANONICAL_LICENSE in header_text)


def style_matches(path: Path, header_lines: list[str]) -> bool:
    if path.name == 'Makefile' or path.suffix in HASH_STYLE_EXTS:
        return any(line.startswith('#') for line in header_lines)
    if path.suffix in C_STYLE_EXTS:
        return header_lines and header_lines[0].strip().startswith('/*')
    if path.suffix == '.rs':
        if not header_lines:
            return False
        first = header_lines[0].strip()
        return first.startswith('/*') or first.startswith('//')
    if path.suffix in HTML_STYLE_EXTS:
        return header_lines and header_lines[0].strip().startswith('<!--')
    return True


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description='Valida cabeçalhos de licença RMR.')
    parser.add_argument('--strict-canonical', action='store_true',
                        help='Exige texto canônico (autor/anos/licença) no cabeçalho.')
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    errors = []

    for root in ROOTS:
        if not root.exists():
            continue
        for path in sorted(root.rglob('*')):
            if not should_check(path):
                continue
            text = path.read_text(encoding='utf-8', errors='ignore')
            lines = text.splitlines()
            header_lines = extract_header_lines(path, lines)
            header_text = '\n'.join(header_lines)

            if not style_matches(path, header_lines):
                errors.append(f'INVALID_HEADER_STYLE: {path}')
                continue

            if not has_any_rmr_header(header_text):
                errors.append(f'MISSING_HEADER: {path}')
                continue

            if args.strict_canonical and not has_canonical_header(header_text):
                errors.append(f'NON_CANONICAL_HEADER: {path}')

    specific = Path('rmr/fix_geom_N_all_scopes.py')
    if specific.exists():
        first = specific.read_text(encoding='utf-8', errors='ignore').splitlines()
        if not first or not first[0].startswith('#!'):
            errors.append(f'MISSING_SHEBANG: {specific}')

    if errors:
        print('\n'.join(errors))
        return 1

    mode = 'strict canonical' if args.strict_canonical else 'compatible'
    print(f'OK: header and shebang checks passed for rmr/ and tools/ ({mode} mode).')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
