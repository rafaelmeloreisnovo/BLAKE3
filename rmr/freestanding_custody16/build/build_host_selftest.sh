#!/usr/bin/env sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-clang}
OUT=${OUT:-/tmp/rmr_fc16_selftest}
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -ffunction-sections -fdata-sections \
  -I"$ROOT/include" "$ROOT"/src/*.c "$ROOT/tests/selftest.c" \
  -Wl,--gc-sections -o "$OUT"
"$OUT"
