#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

FILES="$ROOT/include/rmr_cf140.h $ROOT/kernel/rmr_cf140_ops.c"

grep -En '(^|[^A-Za-z0-9_])(if|for|while)[[:space:]]*\(' $FILES && {
  echo "FAIL control_statement_in_kernel" >&2
  exit 1
}

grep -En 'malloc|calloc|realloc|free[[:space:]]*\(|#include[[:space:]]*<stdio.h>|#include[[:space:]]*<stdlib.h>|#include[[:space:]]*<string.h>|#include[[:space:]]*<time.h>|#include[[:space:]]*<unistd.h>' $FILES && {
  echo "FAIL hosted_or_heap_token_in_kernel" >&2
  exit 1
}

echo "PASS CF140_SOURCE_SHAPE"
