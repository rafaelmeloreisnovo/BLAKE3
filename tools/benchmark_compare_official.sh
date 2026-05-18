#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OFFICIAL_DIR="/tmp/BLAKE3-official-bench"
RUNS=1
TBB_MODE="BOTH"
while [[ $# -gt 0 ]]; do
  case "$1" in
    --official-dir) OFFICIAL_DIR="$2"; shift 2;;
    --runs) RUNS="$2"; shift 2;;
    --tbb) TBB_MODE="$2"; shift 2;;
    *) echo "arg inválido $1"; exit 2;;
  esac
done
for cmd in git cmake ninja python3; do command -v "$cmd" >/dev/null || exit 1; done
rm -rf "$OFFICIAL_DIR" && git clone --depth 1 https://github.com/BLAKE3-team/BLAKE3 "$OFFICIAL_DIR" >/dev/null
python3 - <<PY
import subprocess,time,statistics
runs=int(${RUNS})
repos={'local':'$ROOT_DIR/c','official':'$OFFICIAL_DIR/c'}
base_cfg=[
 ('x86-intrinsics baseline', ['-DBLAKE3_SIMD_TYPE=x86-intrinsics']),
 ('x86-intrinsics avx512-off', ['-DBLAKE3_SIMD_TYPE=x86-intrinsics','-DBLAKE3_NO_AVX512=1']),
 ('amd64-asm baseline', ['-DBLAKE3_SIMD_TYPE=amd64-asm']),
 ('amd64-asm avx512-off', ['-DBLAKE3_SIMD_TYPE=amd64-asm','-DBLAKE3_NO_AVX512=1']),
]
tbb_mode='${TBB_MODE}'.upper()
tbb_values=['OFF','ON'] if tbb_mode=='BOTH' else [tbb_mode]
summary=[]
for tbb in tbb_values:
  for cfg_name,cfg_args in base_cfg:
    row={'cfg':cfg_name,'tbb':tbb,'data':{}}
    for repo,src in repos.items():
      times=[]
      ok=True
      for i in range(runs):
        b=f"{src}/build-bench-{cfg_name.replace(' ','_')}-tbb{tbb}-{i}"
        cmd=['cmake','--fresh','-S',src,'-B',b,'-G','Ninja','-DBLAKE3_TESTING=ON','-DBLAKE3_TESTING_CI=ON',f'-DBLAKE3_USE_TBB={tbb}',*cfg_args]
        t0=time.perf_counter()
        try:
          subprocess.run(cmd,check=True,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
          subprocess.run(['cmake','--build',b,'--target','test'],check=True,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
        except subprocess.CalledProcessError:
          ok=False
          break
        times.append(time.perf_counter()-t0)
      row['data'][repo]=(ok,times)
    summary.append(row)

print('=== Benchmark compare local vs official (com TBB) ===')
for row in summary:
  cfg=f"{row['cfg']} | TBB={row['tbb']}"
  lok,lt=row['data']['local']; ook,ot=row['data']['official']
  print(f"\n[{cfg}]")
  if not lok or not ook:
    print(f" local ok={lok} runs={lt}")
    print(f" official ok={ook} runs={ot}")
    print(' status=SKIP/FAIL (dependência TBB ausente ou falha de build)')
    continue
  la,oa=statistics.mean(lt),statistics.mean(ot)
  delta=((la/oa)-1)*100
  print(f" local   avg={la:.2f}s runs={lt}")
  print(f" official avg={oa:.2f}s runs={ot}")
  print(f" delta local vs official (menor melhor)={delta:+.2f}%")
PY
