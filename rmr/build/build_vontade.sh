#!/data/data/com.termux/files/usr/bin/bash

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -e
. "$(dirname "$0")/profiles.mk"
rmr_select_profile "${RMR_BUILD_PROFILE:-throughput}"

echo "🔨 Sintonizando Frequência N^Δ^n..."
clang -c ../runtime/rafaelia_core.c -o core.o ${RMR_FINAL_CFLAGS}
clang -c ../runtime/kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_vontade ${RMR_FINAL_LDFLAGS}
echo "🚀 Manifestando Vontade Implacável..."
./rafaelia_vontade
