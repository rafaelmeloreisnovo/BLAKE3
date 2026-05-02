#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -e

INC_RMR="-IBLAKE3-master/rmr/include -IBLAKE3-master/c"
CFLAGS="-O3 -ffreestanding -fno-stack-protector -fno-builtin -fPIC"
LDFLAGS="-nostdlib -Wl,-e,_start -pie"

echo "🔨 [OMEGA] ASM..."
clang -c ../runtime/kernel_omega.S -o kernel.o

echo "🔨 [OMEGA] CORE..."
clang -c ../runtime/rafaelia_core.c -o core.o $CFLAGS $INC_RMR

echo "🔨 [OMEGA] BLAKE3 (portable+dispatch+neon)..."
clang -c BLAKE3-master/c/blake3.c -o blake3.o $CFLAGS $INC_RMR
clang -c BLAKE3-master/c/blake3_dispatch.c -o blake3_dispatch.o $CFLAGS $INC_RMR
clang -c BLAKE3-master/c/blake3_portable.c -o blake3_portable.o $CFLAGS $INC_RMR

# Se existir neon no repo:
if [ -f BLAKE3-master/c/blake3_neon.c ]; then
  clang -c BLAKE3-master/c/blake3_neon.c -o blake3_neon.o $CFLAGS $INC_RMR
  OBJS="kernel.o core.o blake3.o blake3_dispatch.o blake3_portable.o blake3_neon.o"
else
  OBJS="kernel.o core.o blake3.o blake3_dispatch.o blake3_portable.o"
fi

echo "🔗 [OMEGA] LINK..."
clang $OBJS -o rafaelia_omega_blake3 $LDFLAGS

echo "🚀 [OMEGA] RUN..."
./rafaelia_omega_blake3
