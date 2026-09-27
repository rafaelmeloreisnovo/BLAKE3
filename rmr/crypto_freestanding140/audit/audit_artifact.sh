#!/usr/bin/env sh
set -eu

BIN=${1:?usage: audit_artifact.sh ELF}
READELF=${READELF:-readelf}

"$READELF" -l "$BIN" | grep -q INTERP && {
  echo "FAIL PT_INTERP" >&2
  exit 1
}

"$READELF" -d "$BIN" 2>/dev/null | grep -q NEEDED && {
  echo "FAIL DT_NEEDED" >&2
  exit 1
}

"$READELF" -Ws "$BIN" |
  awk '$7=="UND" {bad=1; print} END{exit bad}' && :

echo "PASS CF140_FREESTANDING_ARTIFACT"
