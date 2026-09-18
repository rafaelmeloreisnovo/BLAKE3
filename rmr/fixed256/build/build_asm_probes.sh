#!/usr/bin/env sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT=${OUT:-/tmp/rmr_fixed256_asm}
CC=${CC:-clang}
rm -rf "$OUT" && mkdir -p "$OUT"

build_one() {
  name=$1 target=$2 src=$3 extra=$4
  # shellcheck disable=SC2086
  "$CC" --target="$target" -fuse-ld=lld -nostdlib -static -Wl,-e,_start -Wl,--build-id=none -Wl,--no-undefined $extra "$src" -o "$OUT/$name"
  if command -v readelf >/dev/null 2>&1; then
    ! readelf -d "$OUT/$name" 2>/dev/null | grep -q NEEDED
    ! readelf -l "$OUT/$name" 2>/dev/null | grep -q INTERP
    if readelf -Ws "$OUT/$name" 2>/dev/null | awk '$7=="UND" && $4!="NOTYPE" {bad=1} END{exit bad}'; then :; else
      echo "[FAIL] $name unexpected UND" >&2; exit 1
    fi
  fi
  echo "[OK] $name"
}

build_one x86_64  x86_64-none-elf "$ROOT/asm/x86_64/rmr_fixed256.S" "-msse2"
build_one aarch64 aarch64-none-elf "$ROOT/asm/aarch64/rmr_fixed256.S" ""
build_one armv7    armv7a-none-eabi "$ROOT/asm/armv7/rmr_fixed256.S" "-mfpu=neon -mfloat-abi=softfp"
