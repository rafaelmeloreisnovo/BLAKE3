#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

from __future__ import annotations

import copy
import importlib.util
import json
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
VALIDATOR_PATH = ROOT / "rmr/crypto/tools/validate_zip_custody_profile.py"
SPEC = importlib.util.spec_from_file_location("validate_zip_custody_profile", VALIDATOR_PATH)
assert SPEC and SPEC.loader
validator = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(validator)


class ZipCustodyProfileTests(unittest.TestCase):
    def setUp(self) -> None:
        self.profile_path = ROOT / "rmr/crypto/registry/zip_custody_profile.json"
        self.claims_path = ROOT / "rmr/crypto/claims/zip_bitstack_claims.jsonl"
        self.profile = json.loads(self.profile_path.read_text(encoding="utf-8"))

    def test_canonical_profile(self) -> None:
        result = validator.run(self.profile_path, self.claims_path)
        self.assertEqual(result["status"], "PASS")
        self.assertFalse(result["claim_allowed"])
        self.assertEqual(result["layers"], 8)
        self.assertEqual(result["claims"], 8)

    def _assert_profile_rejected(self, mutated: dict) -> None:
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "profile.json"
            path.write_text(json.dumps(mutated), encoding="utf-8")
            with self.assertRaises(validator.ValidationError):
                validator.validate_profile(validator.load_json(path))

    def test_rejects_crc_as_authentication(self) -> None:
        mutated = copy.deepcopy(self.profile)
        mutated["layers"][3]["security_state"] = "AUTHENTICATED_MAC"
        self._assert_profile_rejected(mutated)

    def test_rejects_bit_stacking_as_encryption(self) -> None:
        mutated = copy.deepcopy(self.profile)
        mutated["layers"][1]["security_state"] = "ENCRYPTION"
        self._assert_profile_rejected(mutated)

    def test_rejects_public_word_as_secret(self) -> None:
        mutated = copy.deepcopy(self.profile)
        mutated["word_binding"]["word_is_secret"] = True
        self._assert_profile_rejected(mutated)

    def test_rejects_external_anchor_promotion(self) -> None:
        mutated = copy.deepcopy(self.profile)
        mutated["layers"][7]["implemented_state"] = "VERIFIED"
        self._assert_profile_rejected(mutated)

    def test_rejects_fixture_drift(self) -> None:
        mutated = copy.deepcopy(self.profile)
        mutated["known_fixture"]["payload_sha256"] = "0" * 64
        self._assert_profile_rejected(mutated)

    def test_crc_known_answers(self) -> None:
        self.assertEqual(validator.crc32c(b"123456789"), 0xE3069283)


if __name__ == "__main__":
    unittest.main()
