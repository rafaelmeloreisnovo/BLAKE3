#include <stdio.h>
#include <string.h>

#include "../rafaelia_packet_custody.h"

#define CHECK(condition)                                                   \
  do {                                                                     \
    if (!(condition)) {                                                    \
      fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition);       \
      return 1;                                                            \
    }                                                                      \
  } while (0)

int main(void) {
  uint8_t packet_id[RPC_PACKET_ID_LEN] = {0u};
  const char *payloads[3] = {"alpha", "beta", "gamma"};
  size_t payload_lengths[3] = {5u, 4u, 5u};
  uint64_t manifest_lengths[3] = {5u, 4u, 5u};
  uint8_t digests[3][RPC_DIGEST_LEN];
  uint8_t changed[RPC_DIGEST_LEN];
  uint8_t root_a[RPC_DIGEST_LEN];
  uint8_t root_b[RPC_DIGEST_LEN];
  uint8_t reordered[3][RPC_DIGEST_LEN];
  rpc_stream_state state;
  uint32_t missing = UINT32_MAX;
  uint32_t i;

  for (i = 0u; i < 3u; ++i) {
    CHECK(rpc_chunk_digest(packet_id,
                           i,
                           3u,
                           payloads[i],
                           payload_lengths[i],
                           digests[i]) == RPC_OK);
  }

  CHECK(rpc_chunk_digest(packet_id, 0u, 3u, "alphb", 5u, changed) == RPC_OK);
  CHECK(!rpc_digest_equal(digests[0], changed));

  CHECK(rpc_chunk_digest(packet_id, 1u, 3u, "alpha", 5u, changed) == RPC_OK);
  CHECK(!rpc_digest_equal(digests[0], changed));

  CHECK(rpc_manifest_root(packet_id,
                          3u,
                          manifest_lengths,
                          &digests[0][0],
                          root_a) == RPC_OK);

  memcpy(reordered, digests, sizeof(digests));
  memcpy(reordered[0], digests[1], RPC_DIGEST_LEN);
  memcpy(reordered[1], digests[0], RPC_DIGEST_LEN);
  CHECK(rpc_manifest_root(packet_id,
                          3u,
                          manifest_lengths,
                          &reordered[0][0],
                          root_b) == RPC_OK);
  CHECK(!rpc_digest_equal(root_a, root_b));

  CHECK(rpc_stream_init(&state, packet_id, 3u) == RPC_OK);
  CHECK(rpc_stream_record(&state, 0u, "alpha", 5u, digests[0]) == RPC_OK);
  CHECK(rpc_stream_record(&state, 0u, "alpha", 5u, digests[0]) == RPC_DUPLICATE);
  CHECK(state.received_chunks == 1u);

  CHECK(rpc_stream_record(&state, 1u, "BETA", 4u, digests[1]) == RPC_ERR_DIGEST);
  CHECK(state.rejected_chunks == 1u);
  CHECK(rpc_stream_next_missing(&state, 0u, &missing) == RPC_OK);
  CHECK(missing == 1u);

  CHECK(rpc_stream_record(&state, 2u, "gamma", 5u, digests[2]) == RPC_OK);
  CHECK(!rpc_stream_complete(&state));
  CHECK(rpc_stream_record(&state, 1u, "beta", 4u, digests[1]) == RPC_OK);
  CHECK(rpc_stream_complete(&state));
  CHECK(state.received_chunks == 3u);
  CHECK(state.verified_bytes == 14u);
  CHECK(rpc_stream_next_missing(&state, 0u, &missing) == RPC_INCOMPLETE);

  CHECK(rpc_stream_init(&state, packet_id, 0u) == RPC_ERR_RANGE);
  CHECK(rpc_stream_init(&state, packet_id, RPC_MAX_CHUNKS + 1u) == RPC_ERR_RANGE);

  puts("PASS rafaelia packet custody");
  return 0;
}
