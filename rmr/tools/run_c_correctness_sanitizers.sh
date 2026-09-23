#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OUT="${OUT:-$ROOT/rmr/validation/output/c-sanitizers}"
mkdir -p "$OUT"

(
  cd "$ROOT/c"
  make -f Makefile.testing clean
  CC="${CC:-clang}" make -f Makefile.testing test
  make -f Makefile.testing clean
  CC="${CC:-clang}" make -f Makefile.testing test_asm
) >"$OUT/transcript.txt" 2>&1

echo "C_KAT_ASAN_UBSAN=PASS"
echo "C_INTRINSICS_AND_ASM_VECTORS=PASS"
