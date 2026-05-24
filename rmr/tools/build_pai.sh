#!/usr/bin/env bash
set -euo pipefail

CC_BIN="${CC:-cc}"
CFLAGS_EXTRA="${CFLAGS:-}"
LDFLAGS_EXTRA="${LDFLAGS:-}"

SRC=(
  rmr/core/main.c
  rmr/core/cli.c
  rmr/core/base.c
  rmr/core/geom.c
  rmr/core/toroid.c
  rmr/core/hash_sha256.c
  rmr/core/scan.c
  rmr/core/sign.c
  rmr/core/bench.c
  rmr/core/benchdiff.c
  rmr/core/validate.c
  rmr/core/util.c
  rmr/ui/mode_router.c
  rmr/ui/bbs/menu.c
  rmr/ui/bbs/input.c
  rmr/ui/bbs/screen.c
  rmr/pathcutter/pathcutter.c
  rmr/hwif/detect/detect_fallback.c
  rmr/hwif/detect/detect_x86.c
  rmr/hwif/detect/detect_aarch64.c
)

"$CC_BIN" -D_GNU_SOURCE -std=c11 -O2 -Wall -Wextra -Irmr/include -Irmr/hwif/include ${CFLAGS_EXTRA} -o pai "${SRC[@]}" -lm ${LDFLAGS_EXTRA}
echo "[OK] build: ./pai"
