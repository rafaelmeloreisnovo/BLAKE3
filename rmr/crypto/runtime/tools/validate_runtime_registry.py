#!/usr/bin/env python3
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]
REGISTRY = ROOT / "rmr/crypto/runtime/registry.json"

EXPECTED_ARCH = {
    "portable-any", "x86-32", "x86-64", "armv7", "armv8-32",
    "aarch64", "wasm32", "riscv-portable", "ppc-portable",
}
EXPECTED_ALG = {
    "blake3", "md5", "sha1", "sha256", "sha512", "hmac-sha256",
    "hkdf-sha256", "ed25519", "chacha20-poly1305", "aes-256-gcm",
    "sha3-256", "blake2b-512", "hmac-sha512", "x25519",
    "pbkdf2-hmac-sha256", "sha224", "sha384", "sha3-512",
    "shake128", "shake256", "blake2s-256", "ed448", "x448",
}

def fail(message: str) -> int:
    print(f"FAIL: {message}", file=sys.stderr)
    return 1

def main() -> int:
    data = json.loads(REGISTRY.read_text(encoding="utf-8"))

    if data.get("_meta", {}).get("claim_allowed") is not False:
        return fail("claim_allowed must remain false")

    arch = data.get("architectures", [])
    alg = data.get("algorithms", [])

    arch_ids = {item["id"] for item in arch}
    alg_ids = {item["id"] for item in alg}

    if arch_ids != EXPECTED_ARCH:
        return fail(f"architecture ids mismatch: {sorted(arch_ids)}")
    if alg_ids != EXPECTED_ALG:
        return fail(f"algorithm ids mismatch: {sorted(alg_ids)}")

    for item in arch:
        if not item.get("evidence_paths"):
            return fail(f"{item['id']} missing evidence paths")
        for rel in item["evidence_paths"]:
            if not (ROOT / rel).exists():
                return fail(f"{item['id']} missing evidence path {rel}")

    legacy = {
        item["id"]: item.get("security_use")
        for item in alg
        if item["id"] in {"md5", "sha1"}
    }
    if legacy != {"md5": "compatibility_only", "sha1": "compatibility_only"}:
        return fail("MD5/SHA-1 must stay compatibility_only")

    if any(
        item.get("provider", "").startswith("OpenSSL")
        and item.get("status") != "IMPLEMENTED_PROVIDER"
        for item in alg
    ):
        return fail("OpenSSL algorithms must use IMPLEMENTED_PROVIDER state")

    print("RMR_CRYPTO_RUNTIME_REGISTRY=PASS")
    print(f"architectures={len(arch)} algorithms={len(alg)}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
