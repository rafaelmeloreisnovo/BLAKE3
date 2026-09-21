#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${RMR_CMAKE_BUILD_DIR:-$ROOT/.rmr-build/pai}"
BUILD_TYPE="${RMR_CMAKE_BUILD_TYPE:-Release}"
CC_BIN="${CC:-cc}"
CXX_BIN="${CXX:-}"
JOBS="${JOBS:-2}"

if [ -z "$CXX_BIN" ]; then
  case "$CC_BIN" in
    *clang*) CXX_BIN=clang++ ;;
    *gcc*) CXX_BIN=g++ ;;
    *) CXX_BIN=c++ ;;
  esac
fi

command -v cmake >/dev/null 2>&1 || { echo "missing_command=cmake" >&2; exit 127; }

CMAKE_ARGS=(
  -S "$ROOT/rmr"
  -B "$BUILD_DIR"
  "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
  "-DCMAKE_C_COMPILER=$CC_BIN"
  "-DCMAKE_CXX_COMPILER=$CXX_BIN"
  -DRMR_BUILD_TESTS=ON
)
if command -v ninja >/dev/null 2>&1; then
  CMAKE_ARGS+=(-G Ninja)
fi

cmake "${CMAKE_ARGS[@]}"
cmake --build "$BUILD_DIR" --parallel "$JOBS"
ctest --test-dir "$BUILD_DIR" --output-on-failure
cp "$BUILD_DIR/pai" "$ROOT/pai"
echo "[OK] build: $ROOT/pai"
