# RMR Portable SPDX / provenance rule

New source files in this tree use:

```text
SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
```

Exceptions:
- upstream BLAKE3 provider files under `c/` remain upstream and are never
  relicensed;
- generated receipts/manifests may use metadata fields instead of source
  comments;
- pre-existing RMR files outside this tree retain their prior grant.

The build must never concatenate a new RMR notice onto an upstream file in a
way that implies ownership of the upstream algorithm.
