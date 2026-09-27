# CF140 Secret Buffer Contract V1

## Core idea

“Generate/process the key in the buffer” is interpreted as:
- allocate/reserve the secret workspace before key creation;
- entropy producer writes directly into the final key slot where the platform permits;
- key expansion/derivation uses dedicated in-place scratch lines;
- avoid avoidable intermediate copies;
- wipe all secret-bearing lines at lifecycle end.

## Layout

V1 defines three aligned 64-byte lines:

```text
key   : 64 bytes
work0 : 64 bytes
work1 : 64 bytes
```

This is a substrate, not a universal capacity guarantee. Algorithms requiring larger schedules/workspaces receive algorithm-specific fixed workspaces.

## Alignment

64-byte alignment follows the observed RAFAELIA baseline and common cache-line model.

It is a locality contract, not a claim of cache isolation.

## Secret lifetime

```text
EMPTY
-> ENTROPY_FILLED
-> ACTIVE
-> WIPE
-> EMPTY
```

The pure kernel does not fetch entropy and does not persist secret lifecycle state to logs.

## Zeroization

V1 zeroization:
- uses volatile byte stores;
- is fixed-shape/unrolled;
- emits a compiler memory barrier afterward;
- has no external memset dependency.

A future binary audit must confirm stores survive optimization on each target.

## Key generation boundary

```text
entropy acquisition != key material handling
```

Entropy acquisition belongs to an adapter:
- hardware RNG;
- OS CSPRNG;
- approved secure element/provider.

The kernel accepts the resulting bytes directly into its reserved storage.

No deterministic Q16/CRC/timestamp substitute is permitted for cryptographic entropy.
