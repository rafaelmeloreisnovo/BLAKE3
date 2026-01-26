#!/data/data/com.termux/files/usr/bin/bash
set -e
echo "🔨 [OMEGA] Compilando (ARM64 / no-libc) + ATA(V1)..."
clang -c rafaelia_core.c -o core.o -ffreestanding -O2 -fno-stack-protector
clang -c kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_omega_ata -nostdlib -Wl,-e,_start -pie
echo "🚀 [OMEGA] Executando..."
./rafaelia_omega_ata
