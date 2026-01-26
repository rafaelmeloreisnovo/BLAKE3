#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
echo "🔨 [SYNC] Compilando (ARM64 / no-libc)..."
clang -c sync_omega.c -o sync.o -ffreestanding -O2 -fno-stack-protector -fno-builtin
clang -c sync_omega.S -o sync_asm.o
clang sync_asm.o sync.o -o rafaelia_sync_omega -nostdlib -Wl,-e,_start -pie
echo "🚀 [SYNC] Executando..."
./rafaelia_sync_omega
