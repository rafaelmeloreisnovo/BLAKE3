#!/usr/bin/env sh
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
OUT=${TMPDIR:-/tmp}/rmr_pai42_selftest

cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic \
  "$ROOT/rmr/pai42/src/rmr_pai42.c" \
  "$ROOT/rmr/pai42/tests/rmr_pai42_selftest.c" \
  -o "$OUT"
"$OUT"
python3 "$ROOT/rmr/pai42/tests/test_pai42_bridge.py"
