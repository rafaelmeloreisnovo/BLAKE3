#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-cc}
OUT=${OUT:-/tmp/rmr_sa_selftest}

"$CC"   -std=c11 -O2   -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion   -Wstrict-prototypes -Werror   -I"$ROOT/include"   "$ROOT/src/rmr_standalone_core.c"   "$ROOT/tests/selftest_host.c"   -o "$OUT"

"$OUT"
echo "PASS RMR_STANDALONE_HOST_SELFTEST"
