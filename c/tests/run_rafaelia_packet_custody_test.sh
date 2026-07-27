#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
BUILD_DIR=${RPC_BUILD_DIR:-"$ROOT/build/rafaelia-packet-custody"}
CC_BIN=${CC:-cc}

cmake -S "$ROOT/c" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR"

"$CC_BIN" -std=c99 -Wall -Wextra -Werror -pedantic \
  -I"$ROOT/c" \
  "$ROOT/c/rafaelia_packet_custody.c" \
  "$ROOT/c/tests/rafaelia_packet_custody_test.c" \
  "$BUILD_DIR/libblake3.a" \
  -o "$BUILD_DIR/rafaelia_packet_custody_test"

"$BUILD_DIR/rafaelia_packet_custody_test"
