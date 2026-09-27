#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
CC=${CC:-cc}
NM=${NM:-nm}
OUT=${OUT_DIR:-/tmp/rmr_hash_boundary_v1}

command -v "$CC" >/dev/null 2>&1 || { echo "missing compiler: $CC" >&2; exit 127; }
command -v "$NM" >/dev/null 2>&1 || { echo "missing symbol tool: $NM" >&2; exit 127; }

mkdir -p "$OUT"

CFLAGS="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fvisibility=hidden -fno-ident -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Werror"

"$CC" $CFLAGS -I"$ROOT/rmr/core"   -c "$ROOT/rmr/core/hash_sha256.c"   -o "$OUT/hash_sha256_core.o"

"$CC" $CFLAGS -I"$ROOT/rmr/core" -I"$ROOT/c"   -c "$ROOT/rmr/core/hash_blake3.c"   -o "$OUT/hash_blake3_bytes.o"

"$CC" -std=c11 -O2 -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Werror   -I"$ROOT/rmr/core"   -c "$ROOT/rmr/core/hash_sha256_file.c"   -o "$OUT/hash_sha256_file.o"

"$CC" -std=c11 -O2 -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Werror   -I"$ROOT/rmr/core" -I"$ROOT/c"   -c "$ROOT/rmr/core/hash_blake3_file.c"   -o "$OUT/hash_blake3_file.o"

forbidden='fopen|fread|fwrite|fclose|printf|fprintf|snprintf|malloc|calloc|realloc|free|getenv|time|gmtime|localtime|pthread_create|dlopen|dlsym'

if "$NM" -u "$OUT/hash_sha256_core.o" | grep -Eq "$forbidden"; then
  echo "FAIL sha256_core_hosted_symbol" >&2
  "$NM" -u "$OUT/hash_sha256_core.o" >&2
  exit 1
fi

if "$NM" -u "$OUT/hash_blake3_bytes.o" | grep -Eq "$forbidden"; then
  echo "FAIL blake3_bytes_hosted_symbol" >&2
  "$NM" -u "$OUT/hash_blake3_bytes.o" >&2
  exit 1
fi

if "$NM" -u "$OUT/hash_sha256_core.o" | grep -q .; then
  echo "FAIL sha256_core_unexpected_undefined" >&2
  "$NM" -u "$OUT/hash_sha256_core.o" >&2
  exit 1
fi

if "$NM" -u "$OUT/hash_blake3_bytes.o" |
     awk '{print $NF}' |
     grep -Ev '^(blake3_hasher_init|blake3_hasher_update|blake3_hasher_finalize)$' |
     grep -q .; then
  echo "FAIL blake3_bytes_unexpected_undefined" >&2
  "$NM" -u "$OUT/hash_blake3_bytes.o" >&2
  exit 1
fi

if ! "$NM" -u "$OUT/hash_sha256_file.o" | grep -Eq 'fopen|fread|fclose'; then
  echo "FAIL sha256_file_adapter_boundary_not_observable" >&2
  exit 1
fi

if ! "$NM" -u "$OUT/hash_blake3_file.o" | grep -Eq 'fopen|fread|fclose'; then
  echo "FAIL blake3_file_adapter_boundary_not_observable" >&2
  exit 1
fi

echo "PASS sha256_core=no_hosted_symbols"
echo "PASS blake3_bytes=only_upstream_blake3_symbols"
echo "PASS sha256_file=hosted_adapter_explicit"
echo "PASS blake3_file=hosted_adapter_explicit"
