#!/data/data/com.termux/files/usr/bin/bash
set -e
echo "🔨 Forjando a Existência N^Δ^n..."
clang -c rafaelia_core.c -o core.o -ffreestanding -fno-stack-protector -O3
clang -c kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_beyond -nostdlib -Wl,-e,_start -pie
echo "🚀 Manifestando o Infinito..."
./rafaelia_beyond
