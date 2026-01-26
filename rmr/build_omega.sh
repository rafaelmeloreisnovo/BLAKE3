#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
echo "🔨 [OMEGA] Compilando (ARM64 / no-libc) + ATA..."
clang -c rafaelia_core.c -o core.o -ffreestanding -fno-stack-protector -O2
clang -c kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_omega -nostdlib -Wl,-e,_start -pie
echo "🚀 [OMEGA] Executando..."
./rafaelia_omega
echo "📦 ATA:"
ls -l ATA_OMEGA.bin || true
