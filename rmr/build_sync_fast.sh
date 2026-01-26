#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
echo "🔨 [SYNC_FAST] Compilando (ARM64 / no-libc)..."
clang -c sync_fast.c -o sync_fast.o -ffreestanding -O2 -fno-stack-protector -fno-builtin
clang -c sync_fast.S -o sync_fast_asm.o
clang sync_fast_asm.o sync_fast.o -o rafaelia_sync_omega_fast -nostdlib -Wl,-e,_start -pie
echo "🚀 [SYNC_FAST] Executando..."
./rafaelia_sync_omega_fast
