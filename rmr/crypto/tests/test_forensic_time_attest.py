#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

from __future__ import annotations

import copy
import importlib.util
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
TOOL_PATH = ROOT / "rmr/crypto/tools/forensic_time_attest.py"
SPEC = importlib.util.spec_from_file_location("forensic_time_attest", TOOL_PATH)
assert SPEC and SPEC.loader
tool = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(tool)


def base_record() -> dict:
    return {
        "schema": "rmr.forensic_time.record.v1",
        "evidence_id": "RMR-BLAKE3-MERGE-117",
        "evidence_digest": {
            "algorithm": "GIT-OID-SHA1",
            "value": "e8d3b7bf89b0a6fcb49c752a9cd6b31a860e7147",
        },
        "source_commit": "e8d3b7bf89b0a6fcb49c752a9cd6b31a860e7147",
        "created_at_utc": "2026-09-09T03:19:57Z",
        "created_at_unix_s": 1788923997,
        "local_time_rfc3339": "2026-09-09T00:19:57-03:00",
        "utc_offset": "-03:00",
        "timezone_id": "America/Sao_Paulo",
        "clock_source": "SYSTEM_CLOCK",
        "clock_sync_status": "UNVERIFIED",
        "operator": "Rafael Melo Reis",
        "claim_allowed": False,
    }


class ForensicTimeTests(unittest.TestCase):
    def test_profile_contract(self) -> None:
        profile = tool.load_profile()
        self.assertEqual(profile["profile_id"], "RMR-FORENSIC-TIME-V1")
        self.assertFalse(profile["canonical_time"]["daylight_saving_affects_canonical_time"])
        self.assertFalse(profile["freestanding_custody16"]["core_reads_clock"])

    def test_real_repository_merge_equivalence(self) -> None:
        record = tool.validate_record(base_record())
        self.assertEqual(record["created_at_utc"], "2026-09-09T03:19:57Z")
        self.assertEqual(record["created_at_unix_s"], 1788923997)
        derived = tool.derive(record)
        self.assertEqual(len(derived["instant_digest_sha256"]), 64)
        self.assertEqual(len(derived["local_attestation_digest_sha256"]), 64)
        self.assertEqual(len(derived["signature_preimage_sha256"]), 64)
        self.assertEqual(derived["forensic_state"], "STRUCTURALLY_VALID_LOCAL_ATTESTATION")
        self.assertEqual(derived["digital_signature_state"], "TOKEN_VAZIO")

    def test_rejects_epoch_mismatch(self) -> None:
        record = base_record()
        record["created_at_unix_s"] += 1
        with self.assertRaises(tool.ValidationError):
            tool.validate_record(record)

    def test_rejects_local_instant_mismatch(self) -> None:
        record = base_record()
        record["local_time_rfc3339"] = "2026-09-09T01:19:57-03:00"
        with self.assertRaises(tool.ValidationError):
            tool.validate_record(record)

    def test_rejects_local_time_without_offset(self) -> None:
        record = base_record()
        record["local_time_rfc3339"] = "2026-09-09T00:19:57"
        with self.assertRaises(tool.ValidationError):
            tool.validate_record(record)

    def test_rejects_declared_offset_mismatch(self) -> None:
        record = base_record()
        record["utc_offset"] = "-02:00"
        with self.assertRaises(tool.ValidationError):
            tool.validate_record(record)

    def test_rejects_bad_digest_length(self) -> None:
        record = base_record()
        record["evidence_digest"] = {"algorithm": "SHA-256", "value": "00"}
        with self.assertRaises(tool.ValidationError):
            tool.validate_record(record)

    def test_dst_or_civil_offset_change_does_not_change_instant_digest(self) -> None:
        a = base_record()
        a["evidence_id"] = "RMR-TIME-DST-INVARIANCE"
        a["created_at_utc"] = "2026-01-01T15:00:00Z"
        a["created_at_unix_s"] = 1767279600
        a["local_time_rfc3339"] = "2026-01-01T12:00:00-03:00"
        a["utc_offset"] = "-03:00"
        a["timezone_id"] = "LOCAL_POLICY_A"

        b = copy.deepcopy(a)
        b["local_time_rfc3339"] = "2026-01-01T13:00:00-02:00"
        b["utc_offset"] = "-02:00"
        b["timezone_id"] = "LOCAL_POLICY_B"

        da = tool.derive(a)
        db = tool.derive(b)
        self.assertEqual(da["instant_digest_sha256"], db["instant_digest_sha256"])
        self.assertNotEqual(
            da["local_attestation_digest_sha256"],
            db["local_attestation_digest_sha256"],
        )

    def test_synchronized_rejects_manual_source(self) -> None:
        record = base_record()
        record["clock_source"] = "MANUAL"
        record["clock_sync_status"] = "SYNCHRONIZED"
        with self.assertRaises(tool.ValidationError):
            tool.validate_record(record)

    def test_signature_preimage_is_deterministic(self) -> None:
        one = tool.derive(base_record())
        two = tool.derive(base_record())
        self.assertEqual(one["signature_preimage_sha256"], two["signature_preimage_sha256"])


if __name__ == "__main__":
    unittest.main()
