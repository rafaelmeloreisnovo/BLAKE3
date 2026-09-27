#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
CC=${CC:-cc}
NM=${NM:-nm}
OUT=${OUT_DIR:-/tmp/rmr_pure_core_boundary_v1}

command -v "$CC" >/dev/null 2>&1 || { echo "missing compiler: $CC" >&2; exit 127; }
command -v "$NM" >/dev/null 2>&1 || { echo "missing symbol tool: $NM" >&2; exit 127; }

mkdir -p "$OUT"

CFLAGS="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fvisibility=hidden -fno-ident -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Werror"

"$CC" $CFLAGS -I"$ROOT/rmr/core"   -c "$ROOT/rmr/core/base_prime.c"   -o "$OUT/base_prime.o"

if "$NM" -u "$OUT/base_prime.o" | grep -q .; then
  echo "FAIL base_prime_unexpected_undefined" >&2
  "$NM" -u "$OUT/base_prime.o" >&2
  exit 1
fi

echo "PASS base_prime=pure_integer_core"
