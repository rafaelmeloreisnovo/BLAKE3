#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/manifest.sh"
cd "$SCRIPT_DIR"

clang -c "$RMR_RUNTIME_DIR/sync_fast.c" -o sync_fast.o -ffreestanding -O3 -fno-stack-protector -fno-builtin $RMR_COMMON_CFLAGS
clang -c "$RMR_RUNTIME_DIR/sync_fast.S" -o sync_fast_asm.o
clang sync_fast_asm.o sync_fast.o -o rafaelia_sync_omega_fast -nostdlib -Wl,-e,_start -pie
./rafaelia_sync_omega_fast
