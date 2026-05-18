#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WORKFLOW_FILE="$ROOT_DIR/.github/workflows/ci.yml"
BUILD_DIR="$ROOT_DIR/c/build"
OFFICIAL_DIR=""
OFFICIAL_REPO_URL="https://github.com/BLAKE3-team/BLAKE3"
OFFICIAL_REF=""

SIMD_MATRIX=("x86-intrinsics" "amd64-asm")
TBB_MATRIX=("OFF" "ON")
FULL_MODE=0
AUTO_CLONE=0

usage() {
  cat <<USAGE
Uso: tools/cmake_ci_compare.sh [--full] [--official-dir <path>] [--clone-official [<path>]] [--official-ref <git-ref>]

--full                   Executa matriz completa (SIMD/TBB) igual ao CI.
--official-dir <path>    Pasta do checkout oficial upstream para auditoria de diff.
--clone-official [path]  Clona https://github.com/BLAKE3-team/BLAKE3 para uma pasta (default: /tmp/BLAKE3-official).
--official-ref <ref>     Faz checkout de tag/branch/commit no checkout oficial clonado.
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --full)
      FULL_MODE=1; shift ;;
    --official-dir)
      OFFICIAL_DIR="${2:-}"; shift 2 ;;
    --clone-official)
      AUTO_CLONE=1
      if [[ "${2:-}" != "" && "${2:0:1}" != "-" ]]; then
        OFFICIAL_DIR="$2"
        shift 2
      else
        OFFICIAL_DIR="/tmp/BLAKE3-official"
        shift
      fi
      ;;
    --official-ref)
      OFFICIAL_REF="${2:-}"; shift 2 ;;
    -h|--help)
      usage; exit 0 ;;
    *)
      echo "Argumento inválido: $1" >&2
      usage
      exit 2 ;;
  esac
done

if [[ "$FULL_MODE" -eq 0 ]]; then
  SIMD_MATRIX=("x86-intrinsics")
  TBB_MATRIX=("OFF")
fi

for cmd in cmake ninja git; do
  command -v "$cmd" >/dev/null || { echo "$cmd não encontrado" >&2; exit 1; }
done

clone_official() {
  local dst="$1"
  echo "## Clone do upstream oficial"
  echo "- repo: $OFFICIAL_REPO_URL"
  echo "- dst : $dst"
  rm -rf "$dst"
  git clone --depth 1 "$OFFICIAL_REPO_URL" "$dst"
  if [[ -n "$OFFICIAL_REF" ]]; then
    git -C "$dst" fetch --depth 1 origin "$OFFICIAL_REF"
    git -C "$dst" checkout "$OFFICIAL_REF"
  fi
}

compare_with_official() {
  local official="$1"
  [[ -d "$official/c" ]] || { echo "official inválido: $official (faltando c/)" >&2; exit 1; }

  echo "## Auditoria local vs upstream oficial"
  echo "- local   : $ROOT_DIR/c"
  echo "- official: $official/c"

  local diff_out
  diff_out="$(mktemp)"
  if git --no-pager diff --no-index -- "$official/c" "$ROOT_DIR/c" >"$diff_out"; then
    echo "Sem diferenças no diretório c/ contra upstream informado."
  else
    echo "Diferenças detectadas em c/ contra upstream informado:"
    sed -n '1,200p' "$diff_out"
  fi
  rm -f "$diff_out"
}

run_case() {
  local label="$1"; shift
  echo "[RUN] $label"
  cmake --fresh -S "$ROOT_DIR/c" -B "$BUILD_DIR" -G Ninja "$@"
  cmake --build "$BUILD_DIR" --target test
  ctest --test-dir "$BUILD_DIR" --output-on-failure
}

printf "## Fonte de verdade (workflow)\n"
awk '/cmake_c_tests:/{flag=1} flag{print} /pkg_config_c_tests:/{exit}' "$WORKFLOW_FILE"

if [[ "$AUTO_CLONE" -eq 1 ]]; then
  clone_official "$OFFICIAL_DIR"
fi
if [[ -n "$OFFICIAL_DIR" ]]; then
  printf "\n"
  compare_with_official "$OFFICIAL_DIR"
fi

printf "\n## Execução local\n"
for simd in "${SIMD_MATRIX[@]}"; do
  run_case "SIMD=$simd baseline" -DBLAKE3_TESTING=ON -DBLAKE3_TESTING_CI=ON "-DBLAKE3_SIMD_TYPE=$simd"
  run_case "SIMD=$simd NO_SSE2" -DBLAKE3_TESTING=ON -DBLAKE3_TESTING_CI=ON "-DBLAKE3_SIMD_TYPE=$simd" -DBLAKE3_NO_SSE2=1
  run_case "SIMD=$simd NO_SSE2/NO_SSE41" -DBLAKE3_TESTING=ON -DBLAKE3_TESTING_CI=ON "-DBLAKE3_SIMD_TYPE=$simd" -DBLAKE3_NO_SSE2=1 -DBLAKE3_NO_SSE41=1
  run_case "SIMD=$simd NO_SSE2/NO_SSE41/NO_AVX2" -DBLAKE3_TESTING=ON -DBLAKE3_TESTING_CI=ON "-DBLAKE3_SIMD_TYPE=$simd" -DBLAKE3_NO_SSE2=1 -DBLAKE3_NO_SSE41=1 -DBLAKE3_NO_AVX2=1
  run_case "SIMD=$simd NO_SSE2/NO_SSE41/NO_AVX2/NO_AVX512" -DBLAKE3_TESTING=ON -DBLAKE3_TESTING_CI=ON "-DBLAKE3_SIMD_TYPE=$simd" -DBLAKE3_NO_SSE2=1 -DBLAKE3_NO_SSE41=1 -DBLAKE3_NO_AVX2=1 -DBLAKE3_NO_AVX512=1
done

for tbb in "${TBB_MATRIX[@]}"; do
  run_case "TBB=$tbb" -DBLAKE3_TESTING=ON -DBLAKE3_TESTING_CI=ON "-DBLAKE3_USE_TBB=$tbb"
  echo "[RUN] EXAMPLE TBB=$tbb"
  cmake --fresh -S "$ROOT_DIR/c" -B "$BUILD_DIR" -G Ninja -DBLAKE3_TESTING=ON -DBLAKE3_TESTING_CI=ON -DBLAKE3_EXAMPLES=ON "-DBLAKE3_USE_TBB=$tbb"
  cmake --build "$BUILD_DIR" --target blake3-example
done

echo "Concluído. Exemplo com clone oficial: tools/cmake_ci_compare.sh --clone-official /tmp/BLAKE3-official"
