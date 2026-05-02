#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -e
echo "🔨 Construindo Arquitetura RAFAELIA OMEGA..."

# 1. Compilar C (No-Std)
clang -c ../runtime/rafaelia_core.c -o core.o -ffreestanding -fno-stack-protector -O2

# 2. Compilar ASM (ARM64)
clang -c ../runtime/kernel_omega.S -o kernel.o

# 3. Linkar sem bibliotecas (Puro)
# Usamos -pie para compatibilidade com kernels modernos de Android
clang kernel.o core.o -o rafaelia_beyond -nostdlib -Wl,-e,_start -pie

echo "🚀 Rodando a vontade do infinito..."
./rafaelia_beyond
