#!/usr/bin/env sh
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
CC=${CC:-clang}
LD=${LD:-ld.lld}
OUT=${OUT:-/tmp/rmr_hwif_matrix}

rm -rf "$OUT"
mkdir -p "$OUT"

CFLAGS="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables"

compile_c() {
  target=$1
  name=$2
  defs=$3
  # shellcheck disable=SC2086
  "$CC" --target="$target" $CFLAGS $defs \
    -I"$ROOT/include" -I"$ROOT/hwif/include" \
    -c "$ROOT/hwif/rmr_hwif.c" -o "$OUT/$name.c.o"
}

compile_s() {
  target=$1
  name=$2
  src=$3
  extra=$4
  # shellcheck disable=SC2086
  "$CC" --target="$target" $extra -c "$src" -o "$OUT/$name.s.o"
}

combine() {
  name=$1
  shift
  "$LD" -r "$@" -o "$OUT/$name.o"
  if command -v readelf >/dev/null 2>&1; then
    readelf -Ws "$OUT/$name.o" | awk '
      $7=="UND" && $8!="" {
        print "unexpected UND:", $8 > "/dev/stderr";
        bad=1
      }
      END { exit bad }
    ' || exit 1
  fi
  echo "[PASS] $name"
}

compile_c x86_64-none-elf x86_64 ""
compile_s x86_64-none-elf x86_64 "$ROOT/hwif/asm/x86_64/rmr_hwif_backend.S" ""
combine x86_64 "$OUT/x86_64.c.o" "$OUT/x86_64.s.o"

compile_c aarch64-none-elf aarch64 ""
compile_s aarch64-none-elf aarch64 "$ROOT/hwif/asm/aarch64/rmr_hwif_backend.S" ""
combine aarch64 "$OUT/aarch64.c.o" "$OUT/aarch64.s.o"

compile_c armv7a-linux-gnueabihf armv7_user ""
compile_s armv7a-linux-gnueabihf armv7_user "$ROOT/hwif/asm/armv7/rmr_hwif_user.S" "-march=armv7-a -mthumb"
combine armv7_user "$OUT/armv7_user.c.o" "$OUT/armv7_user.s.o"

compile_c armv7a-linux-gnueabihf armv7_priv "-DRMR_ARMV7_ASSUME_PRIVILEGED=1"
compile_s armv7a-linux-gnueabihf armv7_priv_user "$ROOT/hwif/asm/armv7/rmr_hwif_user.S" "-march=armv7-a -mthumb"
compile_s armv7a-linux-gnueabihf armv7_priv "$ROOT/hwif/asm/armv7/rmr_hwif_privileged.S" "-march=armv7-a"
combine armv7_priv "$OUT/armv7_priv.c.o" "$OUT/armv7_priv_user.s.o" "$OUT/armv7_priv.s.o"

echo "RMR_HWIF_MATRIX=4/4"
echo "EXECUTION_PHYSICAL=TOKEN_VAZIO"
