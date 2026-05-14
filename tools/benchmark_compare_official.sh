#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OFFICIAL_DIR="/tmp/BLAKE3-official-bench"
RUNS=1
while [[ $# -gt 0 ]]; do case "$1" in --official-dir) OFFICIAL_DIR="$2"; shift 2;; --runs) RUNS="$2"; shift 2;; *) echo "arg inválido $1"; exit 2;; esac; done
for cmd in git cmake ninja python3; do command -v "$cmd" >/dev/null || exit 1; done
rm -rf "$OFFICIAL_DIR" && git clone --depth 1 https://github.com/BLAKE3-team/BLAKE3 "$OFFICIAL_DIR" >/dev/null
python3 - <<PY
import subprocess,time,statistics
runs=int(${RUNS})
repos={'local':'$ROOT_DIR/c','official':'$OFFICIAL_DIR/c'}
configs=[
 ('x86-intrinsics baseline', ['-DBLAKE3_SIMD_TYPE=x86-intrinsics']),
 ('x86-intrinsics no_sse2', ['-DBLAKE3_SIMD_TYPE=x86-intrinsics','-DBLAKE3_NO_SSE2=1']),
 ('x86-intrinsics no_sse2_no_sse41', ['-DBLAKE3_SIMD_TYPE=x86-intrinsics','-DBLAKE3_NO_SSE2=1','-DBLAKE3_NO_SSE41=1']),
 ('amd64-asm baseline', ['-DBLAKE3_SIMD_TYPE=amd64-asm']),
 ('amd64-asm no_avx2', ['-DBLAKE3_SIMD_TYPE=amd64-asm','-DBLAKE3_NO_AVX2=1']),
]
summary={}
for cfg_name,cfg_args in configs:
  summary[cfg_name]={}
  for repo,src in repos.items():
    times=[]
    for i in range(runs):
      b=f"{src}/build-bench-{cfg_name.replace(' ','_')}-{i}"
      cmd=['cmake','--fresh','-S',src,'-B',b,'-G','Ninja','-DBLAKE3_TESTING=ON','-DBLAKE3_TESTING_CI=ON',*cfg_args]
      t0=time.perf_counter(); subprocess.run(cmd,check=True,stdout=subprocess.DEVNULL); subprocess.run(['cmake','--build',b,'--target','test'],check=True,stdout=subprocess.DEVNULL); times.append(time.perf_counter()-t0)
    summary[cfg_name][repo]=(statistics.mean(times),statistics.median(times),times)
print('=== Benchmark compare local vs official (cada repo usa seu próprio CMakeLists) ===')
for cfg in configs:
  n=cfg[0]
  la,_,lt=summary[n]['local']; oa,_,ot=summary[n]['official']
  delta=((la/oa)-1)*100
  print(f"\n[{n}]\n local   avg={la:.2f}s runs={lt}\n official avg={oa:.2f}s runs={ot}\n delta local vs official (menor melhor)={delta:+.2f}%")
PY
