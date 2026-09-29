#!/usr/bin/env sh
set -eu

bin=$1

if readelf -l "$bin" | grep -q INTERP; then
  echo "RMR_PORTABLE_FAIL=PT_INTERP"
  exit 1
fi
if readelf -d "$bin" 2>/dev/null | grep -q NEEDED; then
  echo "RMR_PORTABLE_FAIL=DT_NEEDED"
  exit 1
fi
if readelf -Ws "$bin" | awk '$7=="UND" && $4!="NOTYPE" {bad=1} END{exit bad}'; then
  :
else
  echo "RMR_PORTABLE_FAIL=UNEXPECTED_UNDEFINED"
  exit 1
fi

echo "RMR_PORTABLE_ELF_AUDIT=PASS"
sha256sum "$bin"
