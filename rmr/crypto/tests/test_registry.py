#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

"""Self-contained tests for the RMR crypto registry validator."""

from __future__ import annotations

import copy
import importlib.util
import json
import tempfile
from pathlib import Path

MODULE_DIR = Path(__file__).resolve().parents[1]
VALIDATOR_PATH = MODULE_DIR / "tools" / "validate_registry.py"
REGISTRY_PATH = MODULE_DIR / "registry" / "architectures.json"
CLAIMS_PATH = MODULE_DIR / "claims" / "claims.jsonl"


def load_validator():
    spec = importlib.util.spec_from_file_location("rmr_crypto_validator", VALIDATOR_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("unable to load validator")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def write_registry(data: dict) -> Path:
    handle = tempfile.NamedTemporaryFile(
        mode="w", encoding="utf-8", suffix=".json", delete=False
    )
    with handle:
        json.dump(data, handle, ensure_ascii=False, sort_keys=True)
        handle.write("\n")
    return Path(handle.name)


def main() -> int:
    validator = load_validator()

    errors, digest = validator.validate_registry(REGISTRY_PATH)
    assert not errors, f"canonical registry failed: {errors}"
    assert len(digest) == 64 and all(ch in "0123456789abcdef" for ch in digest)

    claims_errors = validator.validate_claims(CLAIMS_PATH)
    assert not claims_errors, f"canonical claims failed: {claims_errors}"
    assert not validator.validate_sha256_kats(), "SHA-256 KATs failed"

    canonical = json.loads(REGISTRY_PATH.read_text(encoding="utf-8"))
    assert len(canonical["families"]) == 10
    assert sum(len(item["candidate_forks"]) for item in canonical["families"]) == 30
    assert all(item["vendored"] is False for item in canonical["families"])

    mutated = copy.deepcopy(canonical)
    mutated["families"][0]["vendored"] = True
    mutated_path = write_registry(mutated)
    try:
        mutation_errors, _ = validator.validate_registry(mutated_path)
        assert any("vendored must be false" in item for item in mutation_errors)
    finally:
        mutated_path.unlink(missing_ok=True)

    mutated = copy.deepcopy(canonical)
    mutated["families"][1]["id"] = mutated["families"][0]["id"]
    mutated_path = write_registry(mutated)
    try:
        mutation_errors, _ = validator.validate_registry(mutated_path)
        assert any("duplicate family id" in item for item in mutation_errors)
    finally:
        mutated_path.unlink(missing_ok=True)

    mutated = copy.deepcopy(canonical)
    mutated["families"][2]["candidate_forks"] = mutated["families"][2][
        "candidate_forks"
    ][:2]
    mutated_path = write_registry(mutated)
    try:
        mutation_errors, _ = validator.validate_registry(mutated_path)
        assert any("expected 3 candidates" in item for item in mutation_errors)
    finally:
        mutated_path.unlink(missing_ok=True)

    print("test_registry: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
