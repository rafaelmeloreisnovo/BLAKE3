#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

RMR_REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OFFICIAL_REPO="${OFFICIAL_REPO:-https://github.com/BLAKE3-team/BLAKE3.git}"
OFFICIAL_REF="${OFFICIAL_REF:-6aab490a26124663329dfd3961b8469f8fdb158b}"

rmr_need() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "missing_command=$1" >&2
    return 127
  }
}

rmr_generator_args() {
  if command -v ninja >/dev/null 2>&1; then
    printf '%s\n' "-G" "Ninja"
  fi
}

rmr_checkout_official() {
  local dest="$1"
  if [ ! -d "$dest/.git" ]; then
    git clone --filter=blob:none --no-tags "$OFFICIAL_REPO" "$dest"
  fi
  git -C "$dest" fetch --depth 1 origin "$OFFICIAL_REF"
  git -C "$dest" checkout --detach FETCH_HEAD
  git -C "$dest" rev-parse HEAD
}

rmr_find_static_lib() {
  find "$1" -type f -name 'libblake3.a' -print -quit
}

rmr_write_common_environment() {
  local out="$1"
  local official_commit="$2"
  {
    echo "schema=RMR-UPSTREAM-V3-ENV"
    echo "observed_at_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "official_repo=$OFFICIAL_REPO"
    echo "official_ref=$OFFICIAL_REF"
    echo "official_commit=$official_commit"
    echo "fork_commit=$(git -C "$RMR_REPO_ROOT" rev-parse HEAD)"
    echo "host=$(uname -a)"
    echo "cc=$(${CC:-clang} --version | head -n1)"
    echo "cmake=$(cmake --version | head -n1)"
    echo "claim_allowed=false"
  } > "$out"
}
