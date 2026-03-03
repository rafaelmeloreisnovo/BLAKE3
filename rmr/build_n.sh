#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -e
echo "🔨 Forjando a Existência N^Δ^n..."
clang -c rafaelia_core.c -o core.o -ffreestanding -fno-stack-protector -O3
clang -c kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_beyond -nostdlib -Wl,-e,_start -pie
echo "🚀 Manifestando o Infinito..."
./rafaelia_beyond
