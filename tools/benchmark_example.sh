#!/usr/bin/env bash
set -euo pipefail

FILE_PATH="${1:-/tmp/bigfile}"
SIZE_BYTES="${2:-1000000000}"

if [[ ! -f "${FILE_PATH}" ]]; then
  echo "Criando arquivo de teste: ${FILE_PATH} (${SIZE_BYTES} bytes)"
  head -c "${SIZE_BYTES}" /dev/zero > "${FILE_PATH}"
fi

echo "SHA-256 (openssl) — ${FILE_PATH}"
time openssl sha256 "${FILE_PATH}"

echo "BLAKE3 (b3sum) — ${FILE_PATH}"
time b3sum "${FILE_PATH}"
