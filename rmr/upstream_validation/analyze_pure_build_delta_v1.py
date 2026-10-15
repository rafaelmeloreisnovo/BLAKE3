#!/usr/bin/env python3
"""Exact-shape pure BLAKE3 2x2 Rust profile and native C paired comparison."""
from __future__ import annotations
import argparse
import csv
import difflib
import json
import math
import random
import statistics
from collections import defaultdict
from pathlib import Path

RUST = ('official_rust_native', 'official_rust_tuned', 'fork_rust_stock', 'fork_rust_native')
C = ('official_c_native', 'fork_c_native')
FIELDS = {'variant', 'size_bytes', 'round', 'iterations', 'nanoseconds', 'digest', 'checksum'}
COMPARISONS = {
    'rust': (
        ('native_fork_vs_official', 'official_rust_native', 'fork_rust_native', 'SOURCE_PLUS_POLICY'),
        ('source_under_stock', 'official_rust_native', 'fork_rust_stock', 'SOURCE_AT_IDENTICAL_STOCK_POLICY'),
        ('source_under_tuned', 'official_rust_tuned', 'fork_rust_native', 'SOURCE_AT_IDENTICAL_TUNED_POLICY'),
        ('official_policy_effect', 'official_rust_native', 'official_rust_tuned', 'POLICY_ONLY_OFFICIAL_SOURCE'),
        ('fork_policy_effect', 'fork_rust_stock', 'fork_rust_native', 'POLICY_ONLY_FORK_SOURCE'),
    ),
    'c': (('native_fork_vs_official', 'official_c_native', 'fork_c_native', 'SOURCE_PLUS_CMAKE_POLICY'),),
}

def percentile(xs, p):
    ys = sorted(xs)
    pos = (len(ys) - 1) * p
    lo, hi = math.floor(pos), math.ceil(pos)
    return ys[lo] if lo == hi else ys[lo] * (hi - pos) + ys[hi] * (pos - lo)

def bootstrap(xs, seed, n=2000):
    rng = random.Random(seed)
    estimates = [statistics.median(xs[rng.randrange(len(xs))] for _ in xs) for _ in range(n)]
    return percentile(estimates, .025), percentile(estimates, .975)

def read_table(path, variants):
    with open(path, encoding='utf8', newline='') as f:
        reader = csv.DictReader(f)
        if set(reader.fieldnames or ()) != FIELDS:
            raise ValueError(f'bad_csv_fields={path}')
        rows = list(reader)
    if not rows:
        raise ValueError(f'empty_csv={path}')
    by_size = defaultdict(lambda: defaultdict(dict))
    digests = defaultdict(set)
    for r in rows:
        if set(r) != FIELDS or r['variant'] not in variants or not all(r.values()):
            raise ValueError('malformed_result_row')
        size, round_, iterations, nanos = (int(r[k]) for k in ('size_bytes', 'round', 'iterations', 'nanoseconds'))
        if size <= 0 or size > 1048576 or round_ <= 0 or iterations <= 0 or nanos <= 0:
            raise ValueError('result_numeric_domain_violation')
        if len(r['digest']) != 64 or any(c not in '0123456789abcdef' for c in r['digest']):
            raise ValueError('digest_format_violation')
        key = (size, round_)
        if r['variant'] in by_size[key]['values']:
            raise ValueError('duplicate_result_row')
        by_size[key]['values'][r['variant']] = (nanos, iterations)
        digests[size].add(r['digest'])
    if any(len(values) != 1 for values in digests.values()):
        raise ValueError('cryptographic_digest_mismatch')
    rounds_by_size = defaultdict(set)
    for (size, round_), record in by_size.items():
        if set(record['values']) != set(variants):
            raise ValueError(f'incomplete_comparison=size:{size},round:{round_}')
        if len({x[1] for x in record['values'].values()}) != 1:
            raise ValueError('iteration_count_mismatch')
        rounds_by_size[size].add(round_)
    for size, rounds in rounds_by_size.items():
        if len(rounds) < 3 or rounds != set(range(1, max(rounds) + 1)):
            raise ValueError(f'invalid_rounds={size}')
    return by_size, digests

def compare_engine(engine, data):
    out = []
    for name, base, challenger, scope in COMPARISONS[engine]:
        for size in sorted({k[0] for k in data}):
            pairs = [v['values'] for (s, _), v in sorted(data.items()) if s == size]
            ratios = [v[base][0] / v[challenger][0] for v in pairs]
            med = statistics.median(ratios)
            lo, hi = bootstrap(ratios, seed=(size ^ sum(ord(c) for c in name)))
            classification = ('RUNNER_LOCAL_GAIN' if lo > 1 else 'RUNNER_LOCAL_LOSS' if hi < 1 else 'UNRESOLVED_OR_PARITY')
            out.append(dict(engine=engine, comparison=name, base=base, challenger=challenger,
                            scope=scope, size_bytes=size, rounds=len(pairs),
                            paired_throughput_ratio_median=med, delta_percent=(med - 1) * 100,
                            ci95_low=lo, ci95_high=hi, classification=classification,
                            claim_allowed=False))
    return out

def compare_commands(args):
    src_a, src_b = (Path(x).resolve() for x in args.commands)
    a, b = (json.loads(x.read_text(encoding='utf8')) for x in (src_a, src_b))
    if not isinstance(a, list) or not isinstance(b, list) or not a or not b:
        raise ValueError('empty_compile_command_set')
    def normalize(arr, root, side):
        rows = []
        for item in arr:
            cmd = item.get('command') or ' '.join(item.get('arguments', []))
            if not cmd or not item.get('file'):
                raise ValueError('compile_command_missing')
            build_dir = str(Path(args.work) / 'c-build' / side)
            cmd = cmd.replace(build_dir, '<BUILD>').replace(str(root), '<SOURCE>')
            file = item['file'].replace(build_dir, '<BUILD>').replace(str(root), '<SOURCE>')
            rows.append(file + ' :: ' + cmd)
        return sorted(rows)
    na = normalize(a, args.official_root, 'official')
    nb = normalize(b, args.fork_root, 'fork')
    diff = list(difflib.unified_diff(na, nb, fromfile='official-effective-cmake', tofile='fork-effective-cmake', lineterm=''))
    Path(args.commands_out).write_text(('\n'.join(diff) if diff else 'C_EFFECTIVE_COMPILE_COMMANDS_EQUAL') + '\n', encoding='utf8')
    print('C_EFFECTIVE_COMPILE_COMMANDS=' + ('DIFFERENT' if diff else 'EQUAL'))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--rust')
    ap.add_argument('--c')
    ap.add_argument('--json')
    ap.add_argument('--md')
    ap.add_argument('--commands', nargs=2)
    ap.add_argument('--official-root')
    ap.add_argument('--fork-root')
    ap.add_argument('--work')
    ap.add_argument('--commands-out')
    a = ap.parse_args()
    if a.commands:
        if not all((a.official_root, a.fork_root, a.work, a.commands_out)):
            ap.error('missing_compile_command_context')
        compare_commands(a)
        return
    if not all((a.rust, a.c, a.json, a.md)):
        ap.error('both_inputs_and_outputs_required')
    rust, rdig = read_table(a.rust, RUST)
    c, cdig = read_table(a.c, C)
    if set(rdig) != set(cdig) or any(rdig[s] != cdig[s] for s in rdig):
        raise ValueError('cross_engine_digest_mismatch')
    results = compare_engine('rust', rust) + compare_engine('c', c)
    result = {'schema': 'RMR-PURE-BUILD-DELTA-V1', 'claim_allowed': False,
              'native_scope': 'HOST_EXECUTION_NO_RMR_NO_JNI', 'results': results,
              'boundaries': ['POLICY_ONLY_REQUIRES_SAME_SOURCE', 'SOURCE_ONLY_REQUIRES_MATCHING_BUILD_POLICY',
                             'CI_RUNNER_PERFORMANCE_NOT_PHYSICAL_ANDROID', 'SHA_EQUALS_INTEGRITY_NOT_AUTHORITY']}
    Path(a.json).write_text(json.dumps(result, sort_keys=True, indent=2) + '\n', encoding='utf8')
    lines = ['# Pure BLAKE3: compile policy × source', '',
             'No JNI, no RMR provider, no custody in the measured hash operation.',
             'Compiled with native CMake policies and explicit 2x2 Cargo release profiles.',
             '', '| Engine | Comparison | Bytes | Fork/tuned delta | Paired 95% CI | Classification |',
             '|---|---|---:|---:|---|---|']
    for r in results:
        lines.append(f"| {r['engine']} | {r['comparison']} | {r['size_bytes']} | {r['delta_percent']:+.3f}% | "
                     f"[{r['ci95_low']:.4f}, {r['ci95_high']:.4f}] | {r['classification']} |")
    lines.extend(['', 'Effects are runner-local; source and policy comparisons must not be mixed.',
                  'A hash equality check does not measure performance. claim_allowed=false.', ''])
    Path(a.md).write_text('\n'.join(lines), encoding='utf8')
    print(f'RMR_PURE_BUILD_ANALYSIS=PASS rows={len(results)}')

if __name__ == '__main__':
    main()
