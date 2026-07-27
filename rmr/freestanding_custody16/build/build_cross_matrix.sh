#!/usr/bin/env sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-clang}
LD=${LD_MODE:--fuse-ld=lld}
BASE="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fvisibility=hidden"
build_one(){ target=$1; flags=$2; start=$3; out=$4; # shellcheck disable=SC2086
  "$CC" --target="$target" $LD $BASE $flags -I"$ROOT/include" "$ROOT"/src/*.c "$ROOT/adapters/termux_linux/rmr_fc16_termux_adapter.c" "$ROOT/adapters/start/$start" -nostdlib -static -Wl,--gc-sections -Wl,--build-id=none -Wl,--no-undefined -Wl,-e,_start -o "$out";
  if readelf -l "$out" | grep -q INTERP; then echo "PT_INTERP: $out" >&2; exit 1; fi
  if readelf -d "$out" 2>/dev/null | grep -q NEEDED; then echo "DT_NEEDED: $out" >&2; exit 1; fi
  readelf -Ws "$out" | awk '$7=="UND" && $4!="NOTYPE" {bad=1} END{exit bad}' || { echo "UND: $out" >&2; exit 1; }
  echo "[OK] $out";
}
build_one x86_64-linux-gnu "-msse4.2" start_x86_64.S /tmp/rmr_fc16_x86_64
build_one aarch64-linux-gnu "-march=armv8-a+crc+simd" start_aarch64.S /tmp/rmr_fc16_aarch64
build_one armv7a-linux-gnueabihf "-march=armv7-a -mthumb -mfpu=neon -mfloat-abi=softfp" start_armv7.S /tmp/rmr_fc16_armv7
