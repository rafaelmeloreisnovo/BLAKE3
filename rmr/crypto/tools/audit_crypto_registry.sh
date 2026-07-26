#!/usr/bin/env sh
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
MODULE_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)

if command -v python3 >/dev/null 2>&1; then
    PYTHON=python3
elif command -v python >/dev/null 2>&1; then
    PYTHON=python
else
    echo "ERROR: Python 3 interpreter not found" >&2
    exit 127
fi

"$PYTHON" "$SCRIPT_DIR/validate_registry.py"
"$PYTHON" "$MODULE_DIR/tests/test_registry.py"

echo "RMR_CRYPTO_AUDIT=PASS"
