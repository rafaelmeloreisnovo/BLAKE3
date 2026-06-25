#!/usr/bin/env sh
# BLAKE3 binary-size matrix probe
#
# Purpose:
#   Produce basic size/section/symbol evidence for C builds.
#   This script is intentionally conservative and avoids claiming performance.
#
# Usage:
#   sh tools/binary_size_matrix.sh
#
# Optional environment:
#   CC=clang sh tools/binary_size_matrix.sh
#   CFLAGS_EXTRA="-Os" sh tools/binary_size_matrix.sh

set -eu

CC=${CC:-cc}
CFLAGS_EXTRA=${CFLAGS_EXTRA:-}
OUT_DIR=${OUT_DIR:-build-size-matrix}
C_DIR=${C_DIR:-c}

mkdir -p "$OUT_DIR"

log() {
  printf '%s\n' "$*"
}

run_optional() {
  label=$1
  shift
  if command -v "$1" >/dev/null 2>&1; then
    "$@" > "$OUT_DIR/$label.txt" 2>&1 || true
  else
    printf 'missing tool: %s\n' "$1" > "$OUT_DIR/$label.txt"
  fi
}

log "compiler: $CC"
run_optional compiler_version "$CC" --version

COMMON="-std=c11 -O3 $CFLAGS_EXTRA -I$C_DIR"

# Portable-only shared object: smallest dependency surface.
$CC $COMMON -shared -fPIC \
  -DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512 \
  -o "$OUT_DIR/libblake3_portable.so" \
  "$C_DIR/blake3.c" "$C_DIR/blake3_dispatch.c" "$C_DIR/blake3_portable.c"

# Portable-only object files for finer per-object analysis.
$CC $COMMON -fPIC -c "$C_DIR/blake3.c" -o "$OUT_DIR/blake3.o"
$CC $COMMON -fPIC -c "$C_DIR/blake3_dispatch.c" \
  -DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512 \
  -o "$OUT_DIR/blake3_dispatch_portable.o"
$CC $COMMON -fPIC -c "$C_DIR/blake3_portable.c" -o "$OUT_DIR/blake3_portable.o"

run_optional size_portable_so size "$OUT_DIR/libblake3_portable.so"
run_optional objdump_h_portable_so objdump -h "$OUT_DIR/libblake3_portable.so"
run_optional nm_portable_so nm -S --size-sort "$OUT_DIR/libblake3_portable.so"
run_optional readelf_symbols_portable_so readelf -Ws "$OUT_DIR/libblake3_portable.so"

if command -v strip >/dev/null 2>&1; then
  cp "$OUT_DIR/libblake3_portable.so" "$OUT_DIR/libblake3_portable_stripped.so"
  strip --strip-unneeded "$OUT_DIR/libblake3_portable_stripped.so" || true
  run_optional size_portable_stripped_so size "$OUT_DIR/libblake3_portable_stripped.so"
fi

# Optional x86_64 Unix assembly build when files/toolchain are compatible.
case "$(uname -m 2>/dev/null || true)" in
  x86_64|amd64)
    if [ -f "$C_DIR/blake3_sse2_x86-64_unix.S" ]; then
      $CC $COMMON -shared -fPIC \
        -o "$OUT_DIR/libblake3_x86_asm.so" \
        "$C_DIR/blake3.c" "$C_DIR/blake3_dispatch.c" "$C_DIR/blake3_portable.c" \
        "$C_DIR/blake3_sse2_x86-64_unix.S" \
        "$C_DIR/blake3_sse41_x86-64_unix.S" \
        "$C_DIR/blake3_avx2_x86-64_unix.S" \
        "$C_DIR/blake3_avx512_x86-64_unix.S" || true
      if [ -f "$OUT_DIR/libblake3_x86_asm.so" ]; then
        run_optional size_x86_asm_so size "$OUT_DIR/libblake3_x86_asm.so"
        run_optional objdump_h_x86_asm_so objdump -h "$OUT_DIR/libblake3_x86_asm.so"
        run_optional nm_x86_asm_so nm -S --size-sort "$OUT_DIR/libblake3_x86_asm.so"
      fi
    fi
    ;;
esac

log "wrote reports to $OUT_DIR"
