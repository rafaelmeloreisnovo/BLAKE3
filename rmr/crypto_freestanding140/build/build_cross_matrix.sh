#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-clang}
OPT=${RMR_CF_OPT:--O2}

"$ROOT/audit/audit_source_shape.sh"

CFLAGS="$OPT -std=c11 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fvisibility=hidden -fno-ident -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Werror"

build_one() {
  target=$1
  extra=$2
  out=$3

  # shellcheck disable=SC2086
  "$CC" --target="$target" -fuse-ld=lld $CFLAGS $extra \
    -I"$ROOT/include" \
    "$ROOT/kernel/rmr_cf140_ops.c" \
    "$ROOT/addresses/H01/kernel/rmr_cf140_sha256_compress.c" \
    "$ROOT/probe/rmr_cf140_probe.c" \
    -nostdlib -static \
    -Wl,--gc-sections \
    -Wl,--icf=safe \
    -Wl,--build-id=none \
    -Wl,--no-undefined \
    -Wl,-e,rmr_cf_probe_entry \
    -Wl,-Map,"$out.map" \
    -o "$out"

  "$ROOT/audit/audit_artifact.sh" "$out"
  echo "target=$target"
  sha256sum "$out"
  size "$out"
}

build_one x86_64-linux-gnu "-march=x86-64" /tmp/rmr_cf140_x86_64
build_one aarch64-linux-gnu "-march=armv8-a" /tmp/rmr_cf140_aarch64
build_one armv7a-linux-gnueabihf "-march=armv7-a -mthumb -mfloat-abi=softfp" /tmp/rmr_cf140_armv7
