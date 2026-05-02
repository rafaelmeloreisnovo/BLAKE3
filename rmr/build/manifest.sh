#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RMR_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
RMR_CORE_DIR="$RMR_ROOT/core"
RMR_RUNTIME_DIR="$RMR_ROOT/runtime"
RMR_HWIF_DIR="$RMR_ROOT/hwif"
RMR_HWIF_INC_DIR="$RMR_HWIF_DIR/include"
RMR_HWIF_ASM_AARCH64_DIR="$RMR_HWIF_DIR/asm/aarch64"
RMR_HWIF_ASM_X86_64_DIR="$RMR_HWIF_DIR/asm/x86_64"
RMR_COMMON_CFLAGS="-I$RMR_CORE_DIR -I$RMR_ROOT/include -I$RMR_HWIF_INC_DIR"
