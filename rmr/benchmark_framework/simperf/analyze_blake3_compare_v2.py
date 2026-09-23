#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# Statistical analysis for the common-harness upstream-vs-fork comparison.
# The analyzer never promotes a universal performance claim.

from __future__ import annotations

import argparse
import csv
import json
import math
import random
import statistics
from collections import defaultdict
from pathlib import Path


def mean(xs):
    return statistics.fmean(xs)


def sample_sd(xs):
    return statistics.stdev(xs) if len(xs) > 1 else 0.0


def cv_percent(xs):
    m = mean(xs)
    return (sample_sd(xs) / m * 100.0) if m else 0.0


def percentile(sorted_values, p):
    if not sorted_values:
        raise ValueError("empty percentile")
    if len(sorted_values) == 1:
        return sorted_values[0]
    pos = (len(sorted_values) - 1) * p
    lo = int(math.floor(pos))
    hi = int(math.ceil(pos))
    if lo == hi:
        return sorted_values[lo]
    frac = pos - lo
    return sorted_values[lo] * (1.0 - frac) + sorted_values[hi] * frac


def bootstrap_paired_median_ratio(pairs, samples, seed):
    rng = random.Random(seed)
    values = []
    n = len(pairs)
    for _ in range(samples):
        draw = [pairs[rng.randrange(n)] for _ in range(n)]
        values.append(statistics.median(fork / official for official, fork in draw))
    values.sort()
    return percentile(values, 0.025), percentile(values, 0.975)


def classify(ci_low, ci_high):
    if ci_low > 1.0:
        return "OBSERVED_FASTER_ON_THIS_RUNNER"
    if ci_high < 1.0:
        return "OBSERVED_SLOWER_ON_THIS_RUNNER"
    return "PARITY_OR_NOISE_NOT_SEPARATED"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--bootstrap-samples", type=int, default=10000)
    args = ap.parse_args()

    rows = list(csv.DictReader(Path(args.csv).open(encoding="utf-8")))
    grouped = defaultdict(lambda: defaultdict(dict))
    digests = defaultdict(set)

    for row in rows:
        size = int(row["size_bytes"])
        round_id = int(row["round"])
        mode = row["mode"]
        mib_s = float(row["mib_s"])
        grouped[size][round_id][mode] = mib_s
        digests[size].add(row["digest"])

    summaries = []
    all_paired_ratios = []

    for size in sorted(grouped):
        rounds = grouped[size]
        pairs = []
        official = []
        fork = []
        for round_id in sorted(rounds):
            point = rounds[round_id]
            if "official" not in point or "fork" not in point:
                raise SystemExit(f"incomplete pair size={size} round={round_id}")
            o = point["official"]
            f = point["fork"]
            official.append(o)
            fork.append(f)
            pairs.append((o, f))
            all_paired_ratios.append(f / o)

        if len(digests[size]) != 1:
            raise SystemExit(f"digest mismatch size={size}")

        median_o = statistics.median(official)
        median_f = statistics.median(fork)
        median_ratio = median_f / median_o
        paired_ratios = [f / o for o, f in pairs]
        paired_median_ratio = statistics.median(paired_ratios)
        ci_low, ci_high = bootstrap_paired_median_ratio(
            pairs,
            args.bootstrap_samples,
            seed=0x524D5200 + size,
        )

        summaries.append({
            "size_bytes": size,
            "rounds": len(pairs),
            "official_median_mib_s": median_o,
            "fork_median_mib_s": median_f,
            "median_ratio_fork_over_official": median_ratio,
            "median_delta_percent": (median_ratio - 1.0) * 100.0,
            "official_mean_mib_s": mean(official),
            "fork_mean_mib_s": mean(fork),
            "mean_ratio_fork_over_official": mean(fork) / mean(official),
            "mean_delta_percent": (mean(fork) / mean(official) - 1.0) * 100.0,
            "official_cv_percent": cv_percent(official),
            "fork_cv_percent": cv_percent(fork),
            "paired_median_ratio": paired_median_ratio,
            "paired_bootstrap_95_low": ci_low,
            "paired_bootstrap_95_high": ci_high,
            "classification": classify(ci_low, ci_high),
            "digest_equivalence": "PASS",
        })

    all_paired_ratios.sort()
    overall = {
        "paired_ratio_median_all_sizes": statistics.median(all_paired_ratios),
        "paired_ratio_mean_all_sizes": mean(all_paired_ratios),
        "paired_ratio_min": min(all_paired_ratios),
        "paired_ratio_max": max(all_paired_ratios),
        "universal_superiority_claim": False,
    }

    output = {
        "schema": "RMR-BLAKE3-UPSTREAM-COMPARE-V2",
        "claim_allowed": False,
        "analysis": summaries,
        "overall": overall,
        "boundaries": [
            "COMMON_HARNESS_RESULT!=UNIVERSAL_SUPERIORITY",
            "RUNNER_OBSERVATION!=DEVICE_FAMILY_CLAIM",
            "MEDIAN_DELTA!=STATISTICAL_SEPARATION",
            "FORK_CORE!=RMR_PIPELINE"
        ],
    }
    Path(args.out).write_text(
        json.dumps(output, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )

    print("RMR_BLAKE3_COMPARE_V2_ANALYSIS=PASS")
    for row in summaries:
        print(
            "size={size_bytes} official={official_median_mib_s:.3f} "
            "fork={fork_median_mib_s:.3f} delta={median_delta_percent:+.3f}% "
            "ci=[{paired_bootstrap_95_low:.6f},{paired_bootstrap_95_high:.6f}] "
            "state={classification}".format(**row)
        )


if __name__ == "__main__":
    main()
