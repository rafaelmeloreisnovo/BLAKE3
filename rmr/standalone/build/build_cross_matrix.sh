#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CC=${CC:-clang}
LD_MODE=${LD_MODE:--fuse-ld=lld}
OPT=${RMR_OPT:--O2}

command -v "$CC" >/dev/null 2>&1 || { echo "missing compiler: $CC" >&2; exit 127; }
command -v ld.lld >/dev/null 2>&1 || { echo "missing linker: ld.lld" >&2; exit 127; }
command -v readelf >/dev/null 2>&1 || { echo "missing auditor: readelf" >&2; exit 127; }

case "$OPT" in
  -O2|-O3|-Os|-Oz) ;;
  *) echo "invalid RMR_OPT=$OPT" >&2; exit 2 ;;
esac

BASE="$OPT -std=c11 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -fvisibility=hidden -fno-ident -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Werror"

build_one() {
  target=$1
  flags=$2
  out=$3

  # shellcheck disable=SC2086
  "$CC" --target="$target" $LD_MODE $BASE $flags     -I"$ROOT/include"     "$ROOT/src/rmr_standalone_core.c"     "$ROOT/probe/rmr_standalone_probe.c"     -nostdlib -static     -Wl,--gc-sections     -Wl,--icf=safe     -Wl,--build-id=none     -Wl,--no-undefined     -Wl,-e,rmr_sa_probe_entry     -Wl,-Map,"$out.map"     -o "$out"

  "$ROOT/audit/audit_artifact.sh" "$out"

  echo "RMR_STANDALONE_RECEIPT_BEGIN"
  echo "target=$target"
  echo "optimization=$OPT"
  sha256sum "$out"
  size "$out"
  defined_symbols=$(readelf -Ws "$out" | awk '$7!="UND" && $8!="" {n++} END{print n+0}')
  echo "defined_symbol_rows=$defined_symbols"
  echo "RMR_STANDALONE_RECEIPT_END"

  echo "[OK] $target -> $out"
}

build_one x86_64-linux-gnu "-march=x86-64" /tmp/rmr_sa_x86_64
build_one aarch64-linux-gnu "-march=armv8-a" /tmp/rmr_sa_aarch64
build_one armv7a-linux-gnueabihf "-march=armv7-a -mthumb -mfloat-abi=softfp" /tmp/rmr_sa_armv7
