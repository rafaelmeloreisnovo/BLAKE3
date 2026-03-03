#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -euo pipefail
echo "🔨 [SYNC] Compilando (ARM64 / no-libc)..."
clang -c sync_omega.c -o sync.o -ffreestanding -O2 -fno-stack-protector -fno-builtin
clang -c sync_omega.S -o sync_asm.o
clang sync_asm.o sync.o -o rafaelia_sync_omega -nostdlib -Wl,-e,_start -pie
echo "🚀 [SYNC] Executando..."
./rafaelia_sync_omega
