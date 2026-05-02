#!/data/data/com.termux/files/usr/bin/sh

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# This file is part of the RMR module.
# It does not modify or replace the BLAKE3 core.

set -eu
cd "$(dirname "$0")/.."
make clean
make CC=clang
echo "[OK] build: ./pai"
