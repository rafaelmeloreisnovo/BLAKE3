#ifndef RAFAELIA_PACKET_CUSTODY_H
#define RAFAELIA_PACKET_CUSTODY_H

#include <stddef.h>
#include <stdint.h>

#include "blake3.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RPC_PACKET_ID_LEN 16u
#define RPC_DIGEST_LEN BLAKE3_OUT_LEN
#define RPC_MAX_CHUNKS 4096u
#define RPC_BITMAP_LEN ((RPC_MAX_CHUNKS + 7u) / 8u)

typedef enum rpc_status {
  RPC_OK = 0,
  RPC_DUPLICATE = 1,
  RPC_INCOMPLETE = 2,
  RPC_ERR_ARGUMENT = -1,
  RPC_ERR_RANGE = -2,
  RPC_ERR_DIGEST = -3
} rpc_status;

typedef struct rpc_stream_state {
  uint8_t packet_id[RPC_PACKET_ID_LEN];
  uint32_t total_chunks;
  uint32_t received_chunks;
  uint32_t rejected_chunks;
  uint64_t verified_bytes;
  uint8_t received_bitmap[RPC_BITMAP_LEN];
} rpc_stream_state;

/*
 * Domain-separated digest for one packet fragment.
 * The digest binds packet identity, fragment index, total fragment count,
 * payload length and payload bytes. It therefore detects reordering,
 * cross-packet substitution, truncation and payload modification when the
 * expected digest comes from an authenticated manifest.
 */
rpc_status rpc_chunk_digest(const uint8_t packet_id[RPC_PACKET_ID_LEN],
                            uint32_t chunk_index,
                            uint32_t total_chunks,
                            const void *payload,
                            size_t payload_len,
                            uint8_t out[RPC_DIGEST_LEN]);

/*
 * Commits to the ordered fragment table. This is a protocol root, not a
 * prediction of the ordinary BLAKE3 digest of an unknown complete message.
 * The root must be authenticated externally (signature, MAC or trusted
 * channel) before it can authorize fragment acceptance.
 */
rpc_status rpc_manifest_root(const uint8_t packet_id[RPC_PACKET_ID_LEN],
                             uint32_t total_chunks,
                             const uint64_t *chunk_lengths,
                             const uint8_t *chunk_digests,
                             uint8_t out[RPC_DIGEST_LEN]);

/* Constant-work comparison over RPC_DIGEST_LEN bytes. */
int rpc_digest_equal(const uint8_t left[RPC_DIGEST_LEN],
                     const uint8_t right[RPC_DIGEST_LEN]);

rpc_status rpc_stream_init(rpc_stream_state *state,
                           const uint8_t packet_id[RPC_PACKET_ID_LEN],
                           uint32_t total_chunks);

/*
 * Verifies and records one fragment. Duplicate valid fragments are
 * idempotent. Invalid fragments increment rejected_chunks and are not marked
 * as received.
 */
rpc_status rpc_stream_record(rpc_stream_state *state,
                             uint32_t chunk_index,
                             const void *payload,
                             size_t payload_len,
                             const uint8_t expected_digest[RPC_DIGEST_LEN]);

rpc_status rpc_stream_next_missing(const rpc_stream_state *state,
                                   uint32_t start_index,
                                   uint32_t *missing_index);

int rpc_stream_complete(const rpc_stream_state *state);

#ifdef __cplusplus
}
#endif

#endif /* RAFAELIA_PACKET_CUSTODY_H */
