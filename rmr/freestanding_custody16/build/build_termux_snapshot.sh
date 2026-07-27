#!/data/data/com.termux/files/usr/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-clang}
OUT=${OUT:-./rmr_fc16_snapshot}
MACHINE=$(uname -m)
ARCH_FLAGS=""
START=""
case "$MACHINE" in
  aarch64) ARCH_FLAGS="-march=armv8-a+crc+simd"; START="$ROOT/adapters/start/start_aarch64.S" ;;
  armv7*|armv8l) ARCH_FLAGS="-march=armv7-a -mthumb -mfpu=neon -mfloat-abi=softfp"; START="$ROOT/adapters/start/start_armv7.S" ;;
  x86_64) ARCH_FLAGS="-msse4.2"; START="$ROOT/adapters/start/start_x86_64.S" ;;
  *) echo "unsupported: $MACHINE" >&2; exit 2 ;;
esac
CFLAGS="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fvisibility=hidden"
# shellcheck disable=SC2086
"$CC" $CFLAGS $ARCH_FLAGS -I"$ROOT/include" "$ROOT"/src/*.c \
  "$ROOT/adapters/termux_linux/rmr_fc16_termux_adapter.c" "$START" \
  -nostdlib -static -Wl,--gc-sections -Wl,--build-id=none -Wl,--no-undefined \
  -Wl,-e,_start -o "$OUT"
printf '%s\n' "[OK] $OUT"
printf '%s\n' "usage: $OUT /directory"
