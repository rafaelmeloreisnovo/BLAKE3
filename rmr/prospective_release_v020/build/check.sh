#!/bin/sh
# Hermetic scratch area; only newly created temp directory is removed.
set -eu
export LC_ALL=C
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/rmr-release-v020.XXXXXXXX")
trap 'rm -rf -- "$WORK"' EXIT HUP INT TERM
CC=${CC:-clang}
FLAGS="-std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-common -fvisibility=hidden -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wstrict-prototypes -Werror"
"$CC" $FLAGS -c "$ROOT/src/rmr_release_evidence_v020.c" -o "$WORK/core.o"
if command -v nm >/dev/null 2>&1; then
  if nm -u "$WORK/core.o" | grep -v '^[[:space:]]*$'; then echo RMR_RELEASE_V020=BLOCKED_UNDEFINED; exit 1; fi
fi
"$CC" $FLAGS "$ROOT/src/rmr_release_evidence_v020.c" "$ROOT/tests/selftest.c" -o "$WORK/selftest"
"$WORK/selftest"
if command -v readelf >/dev/null 2>&1; then
  readelf -h "$WORK/core.o" | grep -E 'Class:|Machine:'
fi
if command -v sha256sum >/dev/null 2>&1; then sha256sum "$WORK/core.o"; fi
printf 'RMR_RELEASE_V020_HOST=PASS_SCOPED\n'
