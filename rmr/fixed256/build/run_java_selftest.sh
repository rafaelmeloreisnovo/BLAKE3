#!/usr/bin/env sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUT=${OUT:-/tmp/rmr_fixed256_java}
rm -rf "$OUT" && mkdir -p "$OUT"
javac -d "$OUT" "$ROOT/java/RmrFixed256.java" "$ROOT/tests/RmrFixed256SelfTest.java"
java -cp "$OUT" RmrFixed256SelfTest
