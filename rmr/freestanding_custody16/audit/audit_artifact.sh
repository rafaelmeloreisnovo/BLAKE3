#!/usr/bin/env sh
set -eu
BIN=${1:?usage: audit_artifact.sh ELF}
[ -f "$BIN" ] || { echo "FAIL missing=$BIN"; exit 1; }
if readelf -l "$BIN" | grep -q INTERP; then echo "FAIL PT_INTERP"; exit 1; fi
if readelf -d "$BIN" 2>/dev/null | grep -q NEEDED; then echo "FAIL DT_NEEDED"; exit 1; fi
readelf -Ws "$BIN" | awk '$7=="UND" && $4!="NOTYPE" {print;bad=1} END{exit bad}' || { echo "FAIL UND"; exit 1; }
for s in malloc calloc realloc free memcpy memset memcmp pthread_create dlopen; do
  if readelf -Ws "$BIN" | grep -Eq "[[:space:]]$s(@|$)"; then echo "FAIL symbol=$s"; exit 1; fi
done
echo "OK FREESTANDING_ARTIFACT_PROVED $BIN"
