# RAFAELIA Verified Packet Custody V1

## Purpose

This sidecar turns low-latency hashing into an early integrity gate for packet or
file fragments. It does **not** alter the BLAKE3 primitive and it does not claim
to predict a digest from unknown bytes.

```text
trusted/authenticated manifest root
        ↓
ordered fragment descriptors
        ↓
receive fragment
        ↓
domain-separated BLAKE3 digest
        ↓
accept | reject | duplicate
        ↓
missing-fragment map
        ↓
selective retransmission
        ↓
complete receipt
```

The useful operational condition is:

```text
hash_rate > arrival_rate
```

When this holds, verification can overlap network or storage delivery instead of
becoming a serial stage after the complete object arrives.

## Cryptographic boundary

BLAKE3 is a hash primitive. The sidecar provides integrity commitments and
fragment accounting. It is not encryption, a key-establishment mechanism or a
digital-signature scheme.

The manifest root must arrive through an authenticated mechanism:

- a trusted local channel;
- a keyed MAC under an established secret;
- a conventional signature;
- or a reviewed post-quantum signature/KEM protocol.

Without authenticated root metadata, an attacker can replace both a fragment and
its expected digest.

The following claims are forbidden:

```text
first fragment + last fragment ⇒ unknown middle
partial bytes ⇒ predicted final BLAKE3 digest
fast hashing ⇒ post-quantum communication protocol
integrity digest ⇒ sender identity
```

If a system predicts the ordinary final digest without the missing message bytes
or their authenticated tree values, that is a cryptographic-break claim and must
be treated as `TOKEN_VAZIO_CRYPTOANALYSIS` until independently reproduced.

## Protocol objects

### Fragment digest

`rpc_chunk_digest` binds:

```text
protocol/version
packet_id[16]
chunk_index
chunk_count
payload_length
payload_bytes
```

This detects, relative to an authenticated expected digest:

- payload modification;
- truncation;
- reordering;
- substitution between packet identities;
- substitution between fragment positions.

### Manifest root

`rpc_manifest_root` commits to the ordered table:

```text
packet_id
chunk_count
(index, length, digest)[0..N-1]
```

This is a RAFAELIA protocol root. It is deliberately not described as the
ordinary BLAKE3 digest of the reassembled payload.

### Fixed-memory stream state

The initial profile supports at most 4,096 fragments and uses a fixed bitmap.
It performs no allocation. A valid duplicate is idempotent; an invalid fragment
is rejected and counted; `rpc_stream_next_missing` locates the next retransmit
candidate.

## Build and integration test

Build the existing C library first:

```sh
cmake -S c -B build/rpc -DCMAKE_BUILD_TYPE=Release
cmake --build build/rpc
```

Compile the sidecar test against the built library:

```sh
cc -std=c99 -Wall -Wextra -Werror -pedantic \
  -Ic \
  c/rafaelia_packet_custody.c \
  c/tests/rafaelia_packet_custody_test.c \
  -Lbuild/rpc -lblake3 \
  -o build/rpc/rafaelia_packet_custody_test

./build/rpc/rafaelia_packet_custody_test
```

Expected output:

```text
PASS rafaelia packet custody
```

## Benchmark contract

No fixed speedup is promoted by this module. Every benchmark record must bind:

```text
commit
compiler and version
flags and linker options
CPU model and microcode
backend/dispatch path
thread count and affinity
input size and alignment
warm-up policy
sample count
p50, p95, p99
cycles/byte
GB/s
coefficient of variation
thermal/frequency state
raw samples or artifact hash
```

A percentage called “stability” is insufficient unless its formula is declared.
Both implementations must use identical rounding and sampling rules.

The primary production metric for this sidecar is not only raw GB/s. It is:

```text
early_reject_latency
verified_bytes_before_transfer_end
selective_retransmit_bytes
false_accepts = 0
false_rejects = 0
queue_high_watermark
```

## Binary and linker investigation

Compiler/linker cleanup can reduce overhead and jitter around hashing through:

- explicit target/dispatch selection;
- symbol visibility and dead-section elimination;
- LTO and inlining inspection;
- alignment and copy avoidance;
- buffer reuse and batching;
- I/O/hash overlap;
- vectorization reports;
- assembly and hardware-counter comparison.

These optimizations may improve delivery-time verification. They do not remove
the cryptographic dependency on all payload bytes or all authenticated fragment
commitments.

## Falsifiers

The design fails if any test shows that:

- a changed payload is accepted under the original expected digest;
- a fragment moved to another index keeps the same digest;
- a reordered manifest keeps the same root;
- an invalid fragment is marked received;
- a duplicate increments received count or verified bytes;
- completion occurs while a fragment is absent;
- a fragment from another packet identity is accepted;
- the claimed manifest root is used without authentication.

## Current state

```text
API and fixed-memory implementation = PRESENT
orchestration harness with API-compatible stub = PASS
actual fork BLAKE3 integration test = TOKEN_VAZIO_PENDING_REPOSITORY_BUILD
benchmark advantage = TOKEN_VAZIO_REPRODUCIBLE_MATRIX
post-quantum protocol = NOT_CLAIMED
claim_allowed = false
```

## Next gate

Run the exact branch against the real C library, preserve stdout/stderr and
binary hashes, then add adversarial fixtures for truncation, reordering,
duplication, packet-ID substitution and selective retransmission.
