#!/usr/bin/env sh
set -eu

REPO_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
RUST_TOOLCHAIN=${RMR_RUST_TOOLCHAIN:-}
TARGETS=${RMR_BAREMETAL_TARGETS:-"thumbv7em-none-eabi armv7a-none-eabi aarch64-unknown-none"}
TMP_ROOT=${RMR_BAREMETAL_TMP_ROOT:-/tmp/rmr_baremetal}
LOCK_CREATED=0

cargo_rmr() {
  if [ -n "$RUST_TOOLCHAIN" ]; then
    cargo +"$RUST_TOOLCHAIN" "$@"
  else
    cargo "$@"
  fi
}

rustc_rmr() {
  if [ -n "$RUST_TOOLCHAIN" ]; then
    rustc +"$RUST_TOOLCHAIN" "$@"
  else
    rustc "$@"
  fi
}

sha256_file() {
  file=$1
  if command -v sha256sum >/dev/null 2>&1; then
    sha256sum "$file" | awk '{print $1}'
  elif command -v shasum >/dev/null 2>&1; then
    shasum -a 256 "$file" | awk '{print $1}'
  else
    printf '%s\n' TOKEN_VAZIO
  fi
}

cleanup() {
  if [ "$LOCK_CREATED" -eq 1 ]; then
    rm -f Cargo.lock
  fi
}
trap cleanup EXIT HUP INT TERM

cd "$REPO_ROOT"
mkdir -p "$TMP_ROOT"

printf 'RMR_RUST_TOOLCHAIN=%s\n' "${RUST_TOOLCHAIN:-ACTIVE_DEFAULT}"
rustc_rmr -Vv
cargo_rmr -V

# Library policy keeps Cargo.lock out of the repository. One execution resolves
# exactly once, hashes that lock, and keeps all subsequent commands --locked.
if [ ! -f Cargo.lock ]; then
  cargo_rmr generate-lockfile
  LOCK_CREATED=1
fi
printf 'RMR_CARGO_LOCK_SHA256=%s\n' "$(sha256_file Cargo.lock)"

META_FILE="$TMP_ROOT/cargo-metadata.json"
cargo_rmr metadata --locked --format-version 1 >"$META_FILE"
printf 'RMR_CARGO_METADATA_SHA256=%s\n' "$(sha256_file "$META_FILE")"

cargo_rmr rmr-freestanding --locked
echo "PASS RMR_RUST_FREESTANDING_NO_STD_PURE_BUILD"

for TARGET in $TARGETS; do
  NEG_LOG="$TMP_ROOT/default_std_${TARGET}.log"

  # Negative gate: a hosted/default-std profile must not silently compile for
  # the declared OS-less target.
  if cargo_rmr build --locked --lib --target "$TARGET" >"$NEG_LOG" 2>&1; then
    echo "FAIL RMR_BAREMETAL_DEFAULT_STD_UNEXPECTEDLY_BUILT target=$TARGET"
    exit 1
  fi
  if ! grep -Eq "can't find crate for .std.|does not support.*std|could not find.*std" "$NEG_LOG"; then
    echo "FAIL RMR_BAREMETAL_NEGATIVE_REASON_UNRESOLVED target=$TARGET"
    cat "$NEG_LOG"
    exit 1
  fi
  echo "PASS RMR_BAREMETAL_REJECTS_DEFAULT_STD target=$TARGET"

  cargo_rmr build --locked --lib --no-default-features --features pure --target "$TARGET"
  echo "PASS RMR_RUST_BAREMETAL_BUILD_GATE target=$TARGET"

  DEPS_DIR="target/$TARGET/debug/deps"
  FOUND=0
  if [ -d "$DEPS_DIR" ]; then
    for RLIB in "$DEPS_DIR"/libblake3-*.rlib; do
      if [ -f "$RLIB" ]; then
        FOUND=1
        printf 'RMR_RLIB_SHA256 target=%s sha256=%s file=%s\n' \
          "$TARGET" "$(sha256_file "$RLIB")" "$RLIB"
      fi
    done
  fi
  if [ "$FOUND" -ne 1 ]; then
    echo "FAIL RMR_BAREMETAL_RLIB_NOT_FOUND target=$TARGET"
    exit 1
  fi
done

echo "PASS RMR_RUST_BAREMETAL_MATRIX targets=$TARGETS"
