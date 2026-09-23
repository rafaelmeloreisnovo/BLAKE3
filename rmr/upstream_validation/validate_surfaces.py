#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PATH = ROOT / "rmr/upstream_validation/surfaces.json"
VALID = {"PASS","FAIL","PENDING_CI","PENDING_CI_PHYSICAL_TOKEN_VAZIO","TOKEN_VAZIO","AUDIT"}

def main():
    data=json.loads(PATH.read_text(encoding="utf-8"))
    assert data["_meta"]["claim_allowed"] is False
    ids=[x["id"] for x in data["surfaces"]]
    assert len(ids)==len(set(ids))
    assert len(ids)>=16
    for row in data["surfaces"]:
        assert row["state"] in VALID, (row["id"],row["state"])
        assert row["gate"]
    print(f"RMR_UPSTREAM_SURFACE_REGISTRY=PASS surfaces={len(ids)}")

if __name__=="__main__":
    main()
