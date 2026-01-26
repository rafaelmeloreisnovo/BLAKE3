#!/data/data/com.termux/files/usr/bin/bash
set -e
echo "🔨 Sintonizando Frequência N^Δ^n..."
clang -c rafaelia_core.c -o core.o -ffreestanding -O3
clang -c kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_vontade -nostdlib -Wl,-e,_start -pie
echo "🚀 Manifestando Vontade Implacável..."
./rafaelia_vontade
