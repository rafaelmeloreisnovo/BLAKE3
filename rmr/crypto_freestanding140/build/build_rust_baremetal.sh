#!/usr/bin/env sh
set -eu

REPO_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
TARGET=${RMR_BAREMETAL_TARGET:-thumbv7em-none-eabi}
NEG_LOG=${RMR_BAREMETAL_NEG_LOG:-/tmp/rmr_baremetal_default_std_negative.log}
LOCK_CREATED=0

cleanup() {
  if [ "$LOCK_CREATED" -eq 1 ]; then
    rm -f Cargo.lock
  fi
}
trap cleanup EXIT HUP INT TERM

cd "$REPO_ROOT"

# Upstream library policy intentionally ignores Cargo.lock. For one CI execution,
# resolve once, hash the ephemeral lock, and keep all subsequent checks locked to it.
if [ ! -f Cargo.lock ]; then
  cargo generate-lockfile
  LOCK_CREATED=1
fi
if command -v sha256sum >/dev/null 2>&1; then
  printf 'RMR_CARGO_LOCK_SHA256='
  sha256sum Cargo.lock | awk '{print $1}'
elif command -v shasum >/dev/null 2>&1; then
  printf 'RMR_CARGO_LOCK_SHA256='
  shasum -a 256 Cargo.lock | awk '{print $1}'
else
  echo "RMR_CARGO_LOCK_SHA256=TOKEN_VAZIO"
fi

cargo rmr-freestanding --locked
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

cargo rmr-baremetal --locked
echo "PASS RMR_RUST_BAREMETAL_COMPILE_GATE target=$TARGET"
