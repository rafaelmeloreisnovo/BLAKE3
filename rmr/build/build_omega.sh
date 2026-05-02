#!/usr/bin/env bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -euo pipefail
. "$(dirname "$0")/profiles.mk"
rmr_select_profile "${RMR_BUILD_PROFILE:-throughput}"


ARCH="$(uname -m)"
CFLAGS="${RMR_FINAL_CFLAGS}"

if [ "$ARCH" = "aarch64" ]; then
  ENTRY="../hwif/asm/aarch64/rmr_entry.S"
  HWIF_ASM="../hwif/asm/aarch64/rmr_hwif_backend.S"
elif [ "$ARCH" = "x86_64" ]; then
  ENTRY="../hwif/asm/x86_64/rmr_entry.S"
  HWIF_ASM="../hwif/asm/x86_64/rmr_hwif_backend.S"
else
  echo "⚠️ arquitetura nao suportada para ASM ($ARCH), usando fallback C sem binario freestanding"
  clang ../runtime/rafaelia_core.c ../hwif/rmr_hwif.c -o rafaelia_omega_host ${RMR_FINAL_CFLAGS}
  ./rafaelia_omega_host
  exit 0
fi

echo "🔨 [OMEGA] Compilando (${ARCH} / no-libc) + dispatcher HWIF..."
clang -c ../runtime/rafaelia_core.c -o core.o ${CFLAGS}
clang -c ../hwif/rmr_hwif.c -o rmr_hwif.o ${CFLAGS}
clang -c "$ENTRY" -o entry.o
clang -c "$HWIF_ASM" -o hwif_backend.o
clang entry.o core.o rmr_hwif.o hwif_backend.o -o rafaelia_omega ${RMR_FINAL_LDFLAGS}

echo "🚀 [OMEGA] Executando..."
./rafaelia_omega

echo "📦 ATA:"
ls -l ATA_OMEGA.bin || true
