#!/usr/bin/env bash
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="${RESULT_ROOT:-$ROOT/benchmark_framework/output/upstream-v3-rmr-integration}"
rm -rf "$OUT"; mkdir -p "$OUT"

python3 "$ROOT/upstream_validation/validate_surfaces.py" | tee "$OUT/surface-registry.txt"
python3 -m py_compile   "$ROOT/benchmark_framework/simperf/simperf_model.py"   "$ROOT/benchmark_framework/simperf/tests/test_simperf.py"   "$ROOT/crypto/runtime/tools/validate_runtime_registry.py"   "$ROOT/crypto/runtime/tools/generate_runtime_matrix.py"

python3 "$ROOT/benchmark_framework/simperf/tests/test_simperf.py"   | tee "$OUT/simperf-selftest.txt"
python3 "$ROOT/crypto/runtime/tools/validate_runtime_registry.py"   | tee "$OUT/crypto-registry.txt"
sh "$ROOT/crypto/runtime/build/build_contract_matrix.sh"   | tee "$OUT/crypto-contract-matrix.txt"

BUILD="$ROOT/.rmr-work/upstream-v3-rmr-cmake"
rm -rf "$BUILD"
cmake -S "$ROOT" -B "$BUILD" -G Ninja   -DCMAKE_BUILD_TYPE=Release   -DRMR_BUILD_TESTS=ON   -DRMR_BUILD_TOPOLOGY=ON   -DRMR_BUILD_HWIF=ON   -DRMR_BUILD_CRYPTO_RUNTIME=ON   -DRMR_STRICT_WARNINGS=ON   >"$OUT/cmake-configure.log" 2>&1
cmake --build "$BUILD" --parallel 2 >"$OUT/cmake-build.log" 2>&1
ctest --test-dir "$BUILD" --output-on-failure | tee "$OUT/ctest.txt"

sh "$ROOT/hwif/build/build_cross_matrix.sh" | tee "$OUT/hwif-matrix.txt"

if [ -f "$ROOT/fixed256/build/build_asm_probes.sh" ]; then
  sh "$ROOT/fixed256/build/build_asm_probes.sh" | tee "$OUT/fixed256-asm.txt"
else
  echo "fixed256_asm=TOKEN_VAZIO_SCRIPT_MISSING" | tee "$OUT/fixed256-asm.txt"
fi

if [ -f "$ROOT/freestanding_custody16/build/build_host_selftest.sh" ]; then
  sh "$ROOT/freestanding_custody16/build/build_host_selftest.sh"     | tee "$OUT/freestanding-host.txt"
else
  echo "freestanding_host=TOKEN_VAZIO_SCRIPT_MISSING" | tee "$OUT/freestanding-host.txt"
fi

if [ -f "$ROOT/freestanding_custody16/build/build_cross_matrix.sh" ]; then
  sh "$ROOT/freestanding_custody16/build/build_cross_matrix.sh"     | tee "$OUT/freestanding-cross.txt"
else
  echo "freestanding_cross=TOKEN_VAZIO_SCRIPT_MISSING" | tee "$OUT/freestanding-cross.txt"
fi

if [ -f "$ROOT/pai42/tests/run_tests.sh" ]; then
  sh "$ROOT/pai42/tests/run_tests.sh" | tee "$OUT/pai42.txt"
else
  echo "pai42=TOKEN_VAZIO_SCRIPT_MISSING" | tee "$OUT/pai42.txt"
fi

TOPO_BIN="$BUILD/rmr-hwif-selftest"
if [ -x "$TOPO_BIN" ]; then
  python3 "$ROOT/tools/rmr_topology_audit.py"     --root "$ROOT/.."     --binary "$TOPO_BIN"     --json-out "$OUT/topology-audit.json"     | tee "$OUT/topology-audit.txt"
else
  echo "topology_binary=TOKEN_VAZIO" | tee "$OUT/topology-audit.txt"
fi

{
  echo "RMR_INTEGRATION_V3=PASS"
  echo "claim_allowed=false"
  echo "physical_arm=TOKEN_VAZIO"
} >"$OUT/receipt.txt"

sha256sum "$OUT"/* >"$OUT/SHA256SUMS.txt"
