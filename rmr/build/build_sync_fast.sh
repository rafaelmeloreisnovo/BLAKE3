#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -euo pipefail
. "$(dirname "$0")/profiles.mk"
rmr_select_profile "${RMR_BUILD_PROFILE:-throughput}"

echo "🔨 [SYNC_FAST] Compilando (ARM64 / no-libc)..."
clang -c ../runtime/sync_fast.c -o sync_fast.o ${RMR_FINAL_CFLAGS}
clang -c ../runtime/sync_fast.S -o sync_fast_asm.o
clang sync_fast_asm.o sync_fast.o -o rafaelia_sync_omega_fast ${RMR_FINAL_LDFLAGS}
echo "🚀 [SYNC_FAST] Executando..."
./rafaelia_sync_omega_fast
