#!/usr/bin/env python3
from pathlib import Path
import sys

ROOTS = (Path('rmr'), Path('tools'))
CHECK_EXTS = {
    '.c', '.h', '.S', '.s', '.sh', '.bash', '.py', '.rb', '.pl',
    '.md', '.txt', '.yaml', '.yml', '.toml', '.json', '.rs', '.ld', '.inc', '.mk'
}
ALLOWLIST = {
    Path('rmr/LICENSE_RMR'),
}
LICENSE_MARKERS = (
    'License: RMR Module License (see rmr/LICENSE_RMR)',
    'License: RMR Module License (see LICENSE_RMR)',
    'Licensed under LICENSE_RMR.',
)


def should_check(path: Path) -> bool:
    if path in ALLOWLIST or path.is_dir():
        return False
    if path.name == 'Makefile':
        return True
    return path.suffix in CHECK_EXTS


def has_license_header(text: str) -> bool:
    head = '\n'.join(text.splitlines()[:16])
    return any(marker in head for marker in LICENSE_MARKERS)


errors = []
for root in ROOTS:
    if not root.exists():
        continue
    for path in sorted(root.rglob('*')):
        if not should_check(path):
            continue
        text = path.read_text(encoding='utf-8', errors='ignore')
        if not has_license_header(text):
            errors.append(f'MISSING LICENSE HEADER: {path}')

# requisito específico
specific = Path('rmr/fix_geom_N_all_scopes.py')
if specific.exists():
    first = specific.read_text(encoding='utf-8', errors='ignore').splitlines()
    if not first or not first[0].startswith('#!'):
        errors.append(f'MISSING SHEBANG: {specific}')

if errors:
    print('\n'.join(errors))
    sys.exit(1)

print('OK: headers and shebang checks passed for rmr/ and tools/.')
