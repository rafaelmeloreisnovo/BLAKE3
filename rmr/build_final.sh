#!/data/data/com.termux/files/usr/bin/bash
set -e
echo "🔨 Forjando RAFAELIA OMEGA em ARM64..."

# Compilação sem dependências
clang -c rafaelia_core.c -o core.o -ffreestanding -fno-stack-protector -O3
clang -c kernel_omega.S -o kernel.o

# Linkagem direta com o ponto de entrada _start
clang kernel.o core.o -o rafaelia_vortex -nostdlib -Wl,-e,_start -pie

echo "🚀 Executando Auto-Identificação e Inflexão..."
./rafaelia_vortex
