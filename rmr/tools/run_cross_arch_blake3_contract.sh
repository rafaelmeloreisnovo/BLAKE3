#!/usr/bin/env sh
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
CC=${CC:-clang}
OUT=${OUT:-"$ROOT/rmr/validation/output/cross-arch"}

rm -rf "$OUT"
mkdir -p "$OUT"

build_one() {
  target=$1
  name=$2
  if "$CC" --target="$target" -std=c11 -ffreestanding -Wall -Wextra -Wpedantic \
      -I"$ROOT/c" -c "$ROOT/rmr/validation/blake3_contract.c" \
      -o "$OUT/$name.o" >"$OUT/$name.log" 2>&1; then
    echo "$name,PASS,$target" >>"$OUT/status.csv"
  else
    echo "$name,TOKEN_VAZIO_TOOLCHAIN,$target" >>"$OUT/status.csv"
  fi
}

echo "profile,state,target" >"$OUT/status.csv"
build_one x86_64-none-elf x86_64
build_one i686-none-elf x86_32
build_one armv7a-none-eabi armv7
build_one aarch64-none-elf aarch64
build_one wasm32-unknown-unknown wasm32
build_one riscv64-none-elf riscv64
build_one powerpc64le-none-elf ppc64le

cat "$OUT/status.csv"
echo "RMR_BLAKE3_CROSS_ARCH_CONTRACT=OBSERVED"
echo "PHYSICAL_EXECUTION=TOKEN_VAZIO_EXCEPT_NATIVE"
