#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
REPO=$(CDPATH= cd -- "$ROOT/../.." && pwd)
OUT=${OUT:-/tmp/rmr_portable_cross}
CC=${CC:-clang}
LD_MODE=${LD_MODE:--fuse-ld=lld}

command -v "$CC" >/dev/null 2>&1 || exit 127
command -v readelf >/dev/null 2>&1 || exit 127
rm -rf "$OUT"
mkdir -p "$OUT"

STRICT="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-common -fvisibility=hidden -ffunction-sections -fdata-sections -fno-ident -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Werror"
UPSTREAM="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-common -fvisibility=hidden -ffunction-sections -fdata-sections -fno-ident -DBLAKE3_NO_SSE2 -DBLAKE3_NO_SSE41 -DBLAKE3_NO_AVX2 -DBLAKE3_NO_AVX512 -DBLAKE3_USE_NEON=0 -DBLAKE3_ATOMICS=0 -DNDEBUG"

build_core() {
  name=$1
  target=$2
  extra=$3
  # shellcheck disable=SC2086
  "$CC" --target="$target" $LD_MODE $STRICT $extra     -I"$ROOT/include"     "$ROOT/src/rmr_portable_v1.c" "$ROOT/probe/rmr_pv1_probe.c"     -nostdlib -static -Wl,--gc-sections -Wl,--build-id=none     -Wl,--no-undefined -Wl,-e,rmr_pv1_probe_entry     -o "$OUT/core-$name"
  sh "$ROOT/audit/audit_artifact.sh" "$OUT/core-$name"
}

build_provider() {
  name=$1
  target=$2
  extra=$3
  resource=$("$CC" -print-resource-dir)
  common="-nostdinc -isystem $resource/include -I$ROOT/provider/blake3/include -I$ROOT/include -I$REPO/c"

  # Authorial adapter/memshim/probe are strict.
  # shellcheck disable=SC2086
  "$CC" --target="$target" $STRICT $extra $common     -c "$ROOT/provider/blake3/rmr_portable_memshim.c"     -o "$OUT/memshim-$name.o"
  # shellcheck disable=SC2086
  "$CC" --target="$target" $STRICT $extra $common     -c "$ROOT/provider/blake3/rmr_portable_blake3_v1.c"     -o "$OUT/adapter-$name.o"
  # shellcheck disable=SC2086
  "$CC" --target="$target" $STRICT $extra $common     -c "$ROOT/probe/rmr_pv1_blake3_probe.c"     -o "$OUT/probe-$name.o"

  # Upstream files keep upstream warning policy.
  for src in blake3.c blake3_dispatch.c blake3_portable.c; do
    # shellcheck disable=SC2086
    "$CC" --target="$target" $UPSTREAM $extra $common       -c "$REPO/c/$src" -o "$OUT/$src-$name.o"
  done

  # shellcheck disable=SC2086
  "$CC" --target="$target" $LD_MODE $extra     "$OUT/memshim-$name.o" "$OUT/adapter-$name.o" "$OUT/probe-$name.o"     "$OUT/blake3.c-$name.o" "$OUT/blake3_dispatch.c-$name.o" "$OUT/blake3_portable.c-$name.o"     -nostdlib -static -Wl,--gc-sections -Wl,--build-id=none     -Wl,--no-undefined -Wl,-e,rmr_pv1_blake3_probe_entry     -o "$OUT/blake3-$name"

  sh "$ROOT/audit/audit_artifact.sh" "$OUT/blake3-$name"
}

build_core x86_64 x86_64-none-elf ""
build_core aarch64 aarch64-none-elf ""
build_core armv7 armv7a-none-eabi "-mthumb -mfpu=neon -mfloat-abi=softfp"

build_provider x86_64 x86_64-none-elf ""
build_provider aarch64 aarch64-none-elf ""
build_provider armv7 armv7a-none-eabi "-mthumb -mfloat-abi=soft"

echo "RMR_PORTABLE_CROSS_MATRIX=PASS"
