#!/usr/bin/env sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-clang}
OUT=${OUT:-/tmp/rmr_fc16_probe}
CFLAGS="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fvisibility=hidden"
ARCH_FLAGS=""
case "$(uname -m 2>/dev/null || echo unknown)" in
  aarch64) ARCH_FLAGS="-march=armv8-a+crc+simd" ;;
  armv7*|armv8l) ARCH_FLAGS="-march=armv7-a -mthumb -mfpu=neon -mfloat-abi=softfp" ;;
  x86_64) ARCH_FLAGS="-msse4.2" ;;
esac
# shellcheck disable=SC2086
"$CC" $CFLAGS $ARCH_FLAGS -I"$ROOT/include" "$ROOT"/src/*.c "$ROOT/probe/freestanding_entry.c" \
  -nostdlib -static -Wl,--gc-sections -Wl,--build-id=none -Wl,--no-undefined \
  -Wl,-e,rmr_fc16_probe_entry -o "$OUT"
if command -v readelf >/dev/null 2>&1; then
  if readelf -d "$OUT" 2>/dev/null | grep -q NEEDED; then echo '[FAIL] DT_NEEDED' >&2; exit 1; fi
  if readelf -l "$OUT" 2>/dev/null | grep -q INTERP; then echo '[FAIL] PT_INTERP' >&2; exit 1; fi
  if readelf -Ws "$OUT" 2>/dev/null | awk '$7=="UND" && $4!="NOTYPE" {bad=1} END{exit bad}'; then :; else echo '[FAIL] unexpected UND' >&2; exit 1; fi
fi
printf '%s\n' "[OK] freestanding probe: $OUT"
