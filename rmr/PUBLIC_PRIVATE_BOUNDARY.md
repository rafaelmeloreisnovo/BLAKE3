<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Public/Private Boundary

Status: CANONICAL_DRAFT
Scope: `rmr/` in this public BLAKE3 fork

## Purpose

The `rmr/` subtree contains public repository material: code, contracts, documentation, validation artifacts, and technical provenance intended to be handled inside this fork's public boundary.

This file prevents accidental mixing between public fork material and private RAFAELIA material held elsewhere.

## Boundary

- Public `rmr/` material is governed by this repository's licenses and notices.
- Private RAFAELIA memory, spiritual artifacts, personal faith records, family data, secrets, and private corpus are not part of this public subtree unless explicitly committed here with proper authority.
- Existence of related private material does not grant permission to publish it here.
- Public technical code is not evidence for private spiritual claims.
- Private spiritual provenance is not a license grant for public code.

## Required Classification Before Import

```yaml
import_gate:
  source_repo: ""
  source_path: ""
  source_ref: ""
  privacy_class: TOKEN_VAZIO
  license_status: TOKEN_VAZIO
  authority: TOKEN_VAZIO
  contains_secret: TOKEN_VAZIO
  contains_sacred_personal_content: TOKEN_VAZIO
  target_path: "rmr/..."
  claim_allowed: false
```

If any field remains TOKEN_VAZIO, do not import.

## Allowed Public Content

- technical source code;
- tests and validation fixtures cleared for public release;
- public documentation;
- public receipts without private payload;
- hashes and opaque references that do not reveal private content.

## Blocked Without Explicit Authorization

- credentials, tokens, keys, private endpoints;
- private spiritual or temple-vivo content;
- personal/family data;
- raw private Drive/corpus material;
- unpublished private audio/data samples;
- private claims promoted as public evidence.

## Evidence Rule

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM.

DOCUMENTED does not mean EXECUTED. PUBLIC does not mean VALIDATED. PRIVATE does not mean PUBLISHABLE. TOKEN_VAZIO blocks promotion.
