#!/usr/bin/env sh
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
CC=${CC:-clang}
OUT=${OUT:-/tmp/rmr_crypto_contract_matrix}

rm -rf "$OUT"
mkdir -p "$OUT"

build_one() {
  target=$1
  name=$2
  "$CC" --target="$target" -std=c11 -ffreestanding -Wall -Wextra -Wpedantic \
    -I"$ROOT/crypto/runtime/include" \
    -c "$ROOT/crypto/runtime/src/rmr_crypto_common.c" \
    -o "$OUT/$name.o"
  echo "[PASS] $name target=$target"
}

build_one x86_64-none-elf x86_64
build_one i686-none-elf x86_32
build_one armv7a-none-eabi armv7
build_one aarch64-none-elf aarch64
build_one wasm32-unknown-unknown wasm32
build_one riscv64-none-elf riscv64_portable
build_one powerpc64le-none-elf ppc64le_portable

echo "RMR_CRYPTO_CONTRACT_MATRIX=7/7"
echo "OPENSSL_CROSS_PROVIDER=TOKEN_VAZIO_PROVIDER_TOOLCHAIN"
