# RMR visual prototypes, RAFSTORE slots and ZIPRAF capsules

## Purpose

This module turns deterministic image descriptors into small labeled prototypes without training or modifying model weights.

```text
image/detector -> RmR_VisionDescriptor -> labeled view -> prototype
prototype -> fixed-slot RmR_VisualStore -> classification candidate
prototype -> canonical binary capsule -> ZIP STORE payload -> ZIPRAF custody
```

## Boundaries

- A prototype is a local exemplar, not a universal object definition.
- CRC32C and the capsule checksum prove deterministic identity, not semantic truth.
- Classification remains `TOKEN_VAZIO` when the minimum score or winner margin is not met.
- The store never allocates heap memory. The caller provides fixed slots.
- The capsule is canonical little-endian and can be stored as a ZIP `STORE` entry.
- A multimodal encoder may be added above this contract later. This version uses the corrected Otsu/foreground/angular descriptor only.

## View geometry

The first six views are the octahedral axes: front, rear, left, right, top and bottom. Ten extra slots cover diagonals and controlled auxiliary views. A prototype rejects duplicate views unless replacement is explicit.

## Persistence

The serialized capsule contains:

- magic/version/total size;
- class id and label;
- view mask and count;
- per-view sample id, source CRC32C and complete visual descriptor;
- prototype CRC32C;
- capsule CRC32C over the complete preceding payload.

The payload is suitable for `dog-0001.rvp` inside `dog-0001.zipraf` using method `STORE`. ZIP packaging is deliberately kept outside the BLAKE3 primitive. RMR owns the capsule and custody metadata; BLAKE3 may hash the resulting artifact.

Structural verification rejects a recomputed-but-invalid capsule when the label/class id disagree, the view mask is inconsistent, a view is duplicated or a view id is outside `0..15`.

## Selection rule

Each query is compared to all eligible views. Lower descriptor distance wins. The exposed score is:

`score_q16 = 65536 - distance_q16`

The store accepts a class only when both conditions hold:

- `best_score >= minimum_score`;
- `best_score - runner_up >= minimum_margin`.

Otherwise the result is `NO_MATCH` or `AMBIGUOUS`, preserving the unknown state.

## Verification

```sh
cc -O2 -std=c11 -Wall -Wextra -Werror -pedantic \
  -Irmr/include rmr/src/rmr_stability.c rmr/src/rmr_visual_prototype.c \
  rmr/tests/rmr_visual_prototype_selftest.c -o rmr_visual_prototype_selftest
./rmr_visual_prototype_selftest
```

Verified fixture result:

```text
rmr_visual_prototype_selftest: OK dog=63395 car=47011 margin=16384 capsule=232
```

A one-view `cachorro` capsule was also materialized as 140 RVC1 bytes, packaged in a 272-byte classic ZIP using method `STORE`, reopened and compared byte-for-byte. The ZIP CRC-32 was `2d8bd598`; the payload SHA-256 was `3438ca62d78667862f86fe809463ce328fc025e763fdd7a8ec1693344726dfed`.
