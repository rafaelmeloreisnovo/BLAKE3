# BLAKE3 export source pointers V2 — 2026-09-24

state=EVIDENCE_ONLY
claim_allowed=false

## Gmail export artefact identifiers

- 2026-01-15 ready email -> zip_id suffix: 2026-01-15-05-24-00-561cb4229c594135a0e0e2521e6a2901.zip
- 2026-01-24 ready email -> zip_id suffix: 2026-01-24-03-39-32-607299083f284dc3bf256459649b7864.zip
- 2026-02-09 ready email -> zip_id suffix: 2026-02-09-10-33-01-736294b3c49d406ca2aa688f61fb7c51.zip
- 2026-07-14 ready email -> zip_id suffix: 2026-07-14-08-27-21-36d73203d79b4c7d80cba74f2c822bf2.zip
- 2026-08-04 ready email -> zip_id suffix: 2026-08-03-07-09-40-dd07c1090553498092e9d2fe77282cd7.zip

All five inspected 2026 links use backend-api/estuary/content.

Exact zip-id searches against current Drive metadata returned no same-name object. This does not establish absence because a downloaded export may have been extracted, renamed, transformed, or not retained as the original ZIP.

## Byte/count observations already preserved in Drive custody

- 2025-07-27 conversations.json: 476336716 bytes; 1664 conversations.
- 2025-08-03 conversations.json: 499885038 bytes; 1788 conversations.
- 2025-09-12 historical archive member conversations.json: 678116200 uncompressed bytes; 2334 conversations.
- later shard corpus conversations-000..050: 1107289897 bytes total; 5054 root objects.

These measurements are not directly interchangeable without normalizing:
- compressed ZIP bytes versus uncompressed JSON bytes;
- single conversations.json versus a shard family;
- included date range;
- object count;
- export schema/version and media members.

## Current gap

TOKEN_VAZIO_CURRENT_SINGLE_EXPORT_09GB:
A current primary single-export object near 0.9 GB has not yet been bound to a provider object, exact byte count and digest in this pass.

## Falsifier / next gate

Locate the candidate current export primary bytes, then record:
1. provider identity / source occurrence;
2. exact byte length;
3. SHA-256 and BLAKE3 if actually computed;
4. JSON/root-object count;
5. min/max create_time and update_time;
6. set difference by conversation_id against prior export.

Do not infer deletion from archive-size comparison alone.
