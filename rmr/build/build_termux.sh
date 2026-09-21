#!/data/data/com.termux/files/usr/bin/sh

# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# Canonical Termux host build for PAI + explicit BLAKE3 adapter.

set -eu
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"
exec ./rmr/tools/build_pai.sh
