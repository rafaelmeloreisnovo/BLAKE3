#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
PURE="$ROOT/src/rmr_portable_v1.c"
JAVA="$ROOT/java/RmrPortableV1.java"
RUST="$ROOT/rust/rmr_portable_v1.rs"

for token in malloc calloc realloc free fopen fread fwrite fclose printf fprintf snprintf open read write socket connect fork exec dlopen dlsym pthread_create; do
  if grep -nE "\\b$token[[:space:]]*\\(" "$PURE"; then
    echo "RMR_PORTABLE_FORBIDDEN_PURE_CALL=$token"
    exit 1
  fi
done

if grep -nE '\b(if|for|while|switch)[[:space:]]*\(' "$PURE"; then
  echo "RMR_PORTABLE_FIXED_CORE_VARIABLE_CONTROL_FLOW=FAIL"
  exit 1
fi

if grep -nE '\b(if|for|while|switch)[[:space:]]*\(' "$JAVA"; then
  echo "RMR_PORTABLE_JAVA_KERNEL_VARIABLE_CONTROL_FLOW=FAIL"
  exit 1
fi

if grep -nE '^[[:space:]]*import[[:space:]]' "$JAVA"; then
  echo "RMR_PORTABLE_JAVA_IMPORT=FAIL"
  exit 1
fi

grep -Fq '#![no_std]' "$RUST" || {
  echo "RMR_PORTABLE_RUST_NOSTD=FAIL"
  exit 1
}

for f in $(find "$ROOT" -type f \( -name '*.c' -o -name '*.h' -o -name '*.rs' -o -name '*.java' \) ! -path '*/provider/blake3/include/*' -print | LC_ALL=C sort); do
  grep -Fq 'LicenseRef-RMR-Individual-Research-1.0' "$f" || {
    echo "RMR_PORTABLE_LICENSE_HEADER_MISSING=$f"
    exit 1
  }
done

echo "RMR_PORTABLE_SOURCE_SHAPE=PASS"
