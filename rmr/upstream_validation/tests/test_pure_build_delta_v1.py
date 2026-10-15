#!/usr/bin/env python3
"""Unit falsifiers for the pure build-delta result parser (no timing claims)."""
import csv
import importlib.util
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[1] / "analyze_pure_build_delta_v1.py"
spec = importlib.util.spec_from_file_location("rmr_pure_analyzer", SCRIPT)
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)

class PureBuildDeltaTests(unittest.TestCase):
    def write_csv(self, directory, variants, *, omit=None, bad_digest=False):
        path = Path(directory) / "rows.csv"
        with path.open("w", newline="", encoding="utf8") as output:
            writer = csv.writer(output)
            writer.writerow(["variant","size_bytes","round","iterations","nanoseconds","digest","checksum"])
            for size in (64, 1024, 1048576):
                for rd in range(1, 4):
                    for index, variant in enumerate(variants):
                        if (size, rd, variant) == omit:
                            continue
                        digest = ("0" * 64) if bad_digest and index == 1 else ("a" * 64)
                        writer.writerow((variant,size,rd,100,1000000 + index*200 + rd*10,digest,"0"))
        return path

    def test_complete_control_matrix(self):
        with tempfile.TemporaryDirectory() as d:
            rows, digests = mod.read_table(self.write_csv(d, mod.RUST), mod.RUST)
            self.assertEqual(len(rows), 9)
            self.assertEqual(len(digests), 3)
            comparisons = mod.compare_engine("rust", rows)
            self.assertEqual(len(comparisons), 15)
            self.assertTrue(all(not r["claim_allowed"] for r in comparisons))

    def test_missing_variant_is_not_pass(self):
        with tempfile.TemporaryDirectory() as d:
            path = self.write_csv(d, mod.RUST, omit=(64, 1, "fork_rust_native"))
            with self.assertRaisesRegex(ValueError, "incomplete_comparison"):
                mod.read_table(path, mod.RUST)

    def test_digest_mismatch_is_not_pass(self):
        with tempfile.TemporaryDirectory() as d:
            path = self.write_csv(d, mod.C, bad_digest=True)
            with self.assertRaisesRegex(ValueError, "cryptographic_digest_mismatch"):
                mod.read_table(path, mod.C)

if __name__ == "__main__":
    unittest.main()
