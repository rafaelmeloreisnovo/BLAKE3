#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
REPO=$(CDPATH= cd -- "$ROOT/../.." && pwd)
OUT=${OUT:-/tmp/rmr_portable_host}
CC=${CC:-clang}

rm -rf "$OUT"
mkdir -p "$OUT"

CFLAGS="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-common -fvisibility=hidden -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Werror"

# shellcheck disable=SC2086
"$CC" $CFLAGS -I"$ROOT/include"   "$ROOT/src/rmr_portable_v1.c"   "$ROOT/tests/selftest_core.c"   -o "$OUT/core_selftest"
"$OUT/core_selftest"

# Provider-backed BLAKE3: algorithm source stays in repository upstream c/.
# shellcheck disable=SC2086
"$CC" -std=c11 -O2 -Wall -Wextra -Wshadow -Werror   -DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512   -DBLAKE3_USE_NEON=0 -DBLAKE3_ATOMICS=0 -DNDEBUG   -I"$ROOT/include" -I"$REPO/c"   "$ROOT/provider/blake3/rmr_portable_blake3_v1.c"   "$REPO/c/blake3.c" "$REPO/c/blake3_dispatch.c" "$REPO/c/blake3_portable.c"   "$ROOT/tests/selftest_blake3.c"   -o "$OUT/blake3_selftest"
"$OUT/blake3_selftest"

if command -v javac >/dev/null 2>&1 && command -v java >/dev/null 2>&1; then
  mkdir -p "$OUT/java"
  javac -d "$OUT/java"     "$ROOT/java/RmrPortableV1.java"     "$ROOT/tests/RmrPortableV1SelfTest.java"
  java -cp "$OUT/java" RmrPortableV1SelfTest
else
  echo "JAVA_SELFTEST=NOT_RUN"
fi

if command -v rustc >/dev/null 2>&1; then
  rustc --crate-name rmr_portable_v1     --crate-type lib --edition 2024     "$ROOT/rust/rmr_portable_v1.rs"     -o "$OUT/librmr_portable_v1.rlib"
  echo "RUST_NO_STD_COMPILE=PASS"
else
  echo "RUST_NO_STD_COMPILE=NOT_RUN"
fi

echo "RMR_PORTABLE_HOST_SELFTEST=PASS"
