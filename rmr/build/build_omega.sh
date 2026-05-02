#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/manifest.sh"
cd "$SCRIPT_DIR"

ARCH="$(uname -m)"
CFLAGS="-ffreestanding -fno-stack-protector -fno-builtin -O3 $RMR_COMMON_CFLAGS"

if [ "$ARCH" = "aarch64" ]; then
  ENTRY="$RMR_HWIF_ASM_AARCH64_DIR/rmr_entry.S"
  HWIF_ASM="$RMR_HWIF_ASM_AARCH64_DIR/rmr_hwif_backend.S"
elif [ "$ARCH" = "x86_64" ]; then
  ENTRY="$RMR_HWIF_ASM_X86_64_DIR/rmr_entry.S"
  HWIF_ASM="$RMR_HWIF_ASM_X86_64_DIR/rmr_hwif_backend.S"
else
  echo "arquitetura nao suportada para ASM ($ARCH)"
  exit 1
fi

clang -c "$RMR_RUNTIME_DIR/rafaelia_core.c" -o core.o $CFLAGS
clang -c "$RMR_HWIF_DIR/rmr_hwif.c" -o rmr_hwif.o $CFLAGS
clang -c "$ENTRY" -o entry.o
clang -c "$HWIF_ASM" -o hwif_backend.o
clang entry.o core.o rmr_hwif.o hwif_backend.o -o rafaelia_omega -nostdlib -Wl,-e,_start -pie
./rafaelia_omega
