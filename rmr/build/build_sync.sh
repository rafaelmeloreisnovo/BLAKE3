#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/manifest.sh"
cd "$SCRIPT_DIR"

clang -c "$RMR_RUNTIME_DIR/sync_omega.c" -o sync.o -ffreestanding -O3 -fno-stack-protector -fno-builtin $RMR_COMMON_CFLAGS
clang -c "$RMR_RUNTIME_DIR/sync_omega.S" -o sync_asm.o
clang sync_asm.o sync.o -o rafaelia_sync_omega -nostdlib -Wl,-e,_start -pie
./rafaelia_sync_omega
