#!/usr/bin/env sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-cc}
OUT=${OUT:-/tmp/rmr_cf140_selftest}
SHA_OUT=${SHA_OUT:-/tmp/rmr_cf140_sha256_kat}

"$CC" -std=c11 -O2 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Werror \
  -I"$ROOT/include" \
  "$ROOT/kernel/rmr_cf140_ops.c" \
  "$ROOT/tests/selftest_host.c" \
  -o "$OUT"
"$OUT"
echo "PASS CF140_HOST_SELFTEST"

"$CC" -std=c11 -O2 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Werror \
  -I"$ROOT/include" \
  "$ROOT/kernel/rmr_cf140_ops.c" \
  "$ROOT/addresses/H01/kernel/rmr_cf140_sha256_compress.c" \
  "$ROOT/tests/selftest_sha256_host.c" \
  -o "$SHA_OUT"
"$SHA_OUT"
echo "PASS CF140_SHA256_ABC_COMPRESSION_KAT"
