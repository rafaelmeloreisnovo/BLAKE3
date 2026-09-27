#!/usr/bin/env sh
set -eu

BIN=${1:?usage: audit_artifact.sh ELF}
[ -f "$BIN" ] || { echo "FAIL missing=$BIN" >&2; exit 1; }

READELF=${READELF:-readelf}

if "$READELF" -l "$BIN" 2>/dev/null | grep -q INTERP; then
  echo "FAIL PT_INTERP $BIN" >&2
  exit 1
fi

if "$READELF" -d "$BIN" 2>/dev/null | grep -q NEEDED; then
  echo "FAIL DT_NEEDED $BIN" >&2
  exit 1
fi

"$READELF" -Ws "$BIN" 2>/dev/null |
  awk '$7=="UND" && $4!="NOTYPE" {print; bad=1} END{exit bad}' ||
  { echo "FAIL unexpected_UND $BIN" >&2; exit 1; }

for symbol in   malloc calloc realloc free   printf fprintf snprintf   fopen fread fwrite fclose   pthread_create dlopen dlsym   getenv time gmtime localtime
do
  if "$READELF" -Ws "$BIN" 2>/dev/null |
       grep -Eq "[[:space:]]$symbol(@|$)"
  then
    echo "FAIL forbidden_symbol=$symbol $BIN" >&2
    exit 1
  fi
done

echo "PASS FREESTANDING_ARTIFACT $BIN"
