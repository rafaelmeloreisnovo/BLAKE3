#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -euo pipefail
. "$(dirname "$0")/profiles.mk"
rmr_select_profile "${RMR_BUILD_PROFILE:-throughput}"

echo "🔨 [SYNC] Compilando (ARM64 / no-libc)..."
clang -c ../runtime/sync_omega.c -o sync.o ${RMR_FINAL_CFLAGS}
clang -c ../runtime/sync_omega.S -o sync_asm.o
clang sync_asm.o sync.o -o rafaelia_sync_omega ${RMR_FINAL_LDFLAGS}
echo "🚀 [SYNC] Executando..."
./rafaelia_sync_omega
