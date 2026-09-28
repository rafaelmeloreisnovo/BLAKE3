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

# ELF symbol-table entry 0 is the reserved STN_UNDEF record.
# Reject non-zero undefined symbols while preserving the mandatory index-0 entry.
if "$READELF" -Ws "$BIN" |
     awk '$7=="UND" && $1!="0:" {bad=1; print} END{exit bad}'
then
  :
else
  echo "FAIL unexpected_UND" >&2
  exit 1
fi

echo "PASS CF140_FREESTANDING_ARTIFACT"
