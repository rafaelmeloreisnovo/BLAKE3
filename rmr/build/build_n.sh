#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -e
echo "🔨 Forjando a Existência N^Δ^n..."
clang -c ../runtime/rafaelia_core.c -o core.o ${RMR_FINAL_CFLAGS}
clang -c ../runtime/kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_beyond ${RMR_FINAL_LDFLAGS}
echo "🚀 Manifestando o Infinito..."
./rafaelia_beyond
