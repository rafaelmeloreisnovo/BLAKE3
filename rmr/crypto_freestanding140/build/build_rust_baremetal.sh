#!/usr/bin/env sh
set -eu

REPO_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
TARGET=${RMR_BAREMETAL_TARGET:-thumbv7em-none-eabi}
NEG_LOG=${RMR_BAREMETAL_NEG_LOG:-/tmp/rmr_baremetal_default_std_negative.log}

cd "$REPO_ROOT"

cargo rmr-freestanding
echo "PASS RMR_RUST_FREESTANDING_NO_STD_PURE"

if cargo check --locked --lib --target "$TARGET" >"$NEG_LOG" 2>&1; then
  echo "FAIL RMR_BAREMETAL_DEFAULT_STD_UNEXPECTEDLY_COMPILED"
  exit 1
fi
if ! grep -Eq "can't find crate for .std.|does not support.*std|could not find.*std" "$NEG_LOG"; then
  echo "FAIL RMR_BAREMETAL_NEGATIVE_REASON_UNRESOLVED"
  cat "$NEG_LOG"
  exit 1
fi
echo "PASS RMR_BAREMETAL_REJECTS_DEFAULT_STD"

cargo rmr-baremetal
echo "PASS RMR_RUST_BAREMETAL_COMPILE_GATE target=$TARGET"
