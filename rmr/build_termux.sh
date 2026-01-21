#!/data/data/com.termux/files/usr/bin/sh
set -eu
cd "$(dirname "$0")/.."
make clean
make CC=clang
echo "[OK] build: ./pai"
