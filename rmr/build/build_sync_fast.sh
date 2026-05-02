#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -euo pipefail
echo "🔨 [SYNC_FAST] Compilando (ARM64 / no-libc)..."
clang -c ../runtime/sync_fast.c -o sync_fast.o -ffreestanding -O2 -fno-stack-protector -fno-builtin
clang -c ../runtime/sync_fast.S -o sync_fast_asm.o
clang sync_fast_asm.o sync_fast.o -o rafaelia_sync_omega_fast -nostdlib -Wl,-e,_start -pie
echo "🚀 [SYNC_FAST] Executando..."
./rafaelia_sync_omega_fast
