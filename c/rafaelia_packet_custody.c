#include "rafaelia_packet_custody.h"

#define RPC_CHUNK_CONTEXT "RAFAELIA_PACKET_CUSTODY_CHUNK_V1"
#define RPC_ROOT_CONTEXT  "RAFAELIA_PACKET_CUSTODY_ROOT_V1"

static void rpc_zero(void *ptr, size_t len) {
  uint8_t *p = (uint8_t *)ptr;
  size_t i;
  for (i = 0u; i < len; ++i) {
    p[i] = 0u;
  }
}

static void rpc_copy(void *dst, const void *src, size_t len) {
  uint8_t *d = (uint8_t *)dst;
  const uint8_t *s = (const uint8_t *)src;
  size_t i;
  for (i = 0u; i < len; ++i) {
    d[i] = s[i];
  }
}

static void rpc_store32_le(uint8_t out[4], uint32_t value) {
  out[0] = (uint8_t)value;
  out[1] = (uint8_t)(value >> 8);
  out[2] = (uint8_t)(value >> 16);
  out[3] = (uint8_t)(value >> 24);
}

static void rpc_store64_le(uint8_t out[8], uint64_t value) {
  out[0] = (uint8_t)value;
  out[1] = (uint8_t)(value >> 8);
  out[2] = (uint8_t)(value >> 16);
  out[3] = (uint8_t)(value >> 24);
  out[4] = (uint8_t)(value >> 32);
  out[5] = (uint8_t)(value >> 40);
  out[6] = (uint8_t)(value >> 48);
  out[7] = (uint8_t)(value >> 56);
}

static int rpc_valid_total(uint32_t total_chunks) {
  return total_chunks > 0u && total_chunks <= RPC_MAX_CHUNKS;
}

static int rpc_bit_is_set(const rpc_stream_state *state, uint32_t index) {
  return (state->received_bitmap[index >> 3] &
          (uint8_t)(1u << (index & 7u))) != 0u;
}

static void rpc_bit_set(rpc_stream_state *state, uint32_t index) {
  state->received_bitmap[index >> 3] |=
      (uint8_t)(1u << (index & 7u));
}

rpc_status rpc_chunk_digest(const uint8_t packet_id[RPC_PACKET_ID_LEN],
                            uint32_t chunk_index,
                            uint32_t total_chunks,
                            const void *payload,
                            size_t payload_len,
                            uint8_t out[RPC_DIGEST_LEN]) {
  blake3_hasher hasher;
  uint8_t header[36];

  if (packet_id == NULL || out == NULL ||
      (payload == NULL && payload_len != 0u)) {
    return RPC_ERR_ARGUMENT;
  }
  if (!rpc_valid_total(total_chunks) || chunk_index >= total_chunks) {
    return RPC_ERR_RANGE;
  }

  header[0] = (uint8_t)'R';
  header[1] = (uint8_t)'P';
  header[2] = (uint8_t)'C';
  header[3] = 1u;
  rpc_copy(header + 4u, packet_id, RPC_PACKET_ID_LEN);
  rpc_store32_le(header + 20u, chunk_index);
  rpc_store32_le(header + 24u, total_chunks);
  rpc_store64_le(header + 28u, (uint64_t)payload_len);

  blake3_hasher_init_derive_key(&hasher, RPC_CHUNK_CONTEXT);
  blake3_hasher_update(&hasher, header, sizeof(header));
  if (payload_len != 0u) {
    blake3_hasher_update(&hasher, payload, payload_len);
  }
  blake3_hasher_finalize(&hasher, out, RPC_DIGEST_LEN);
  return RPC_OK;
}

rpc_status rpc_manifest_root(const uint8_t packet_id[RPC_PACKET_ID_LEN],
                             uint32_t total_chunks,
                             const uint64_t *chunk_lengths,
                             const uint8_t *chunk_digests,
                             uint8_t out[RPC_DIGEST_LEN]) {
  blake3_hasher hasher;
  uint8_t header[24];
  uint8_t record_header[12];
  uint32_t i;

  if (packet_id == NULL || chunk_lengths == NULL ||
      chunk_digests == NULL || out == NULL) {
    return RPC_ERR_ARGUMENT;
  }
  if (!rpc_valid_total(total_chunks)) {
    return RPC_ERR_RANGE;
  }

  header[0] = (uint8_t)'R';
  header[1] = (uint8_t)'P';
  header[2] = (uint8_t)'R';
  header[3] = 1u;
  rpc_copy(header + 4u, packet_id, RPC_PACKET_ID_LEN);
  rpc_store32_le(header + 20u, total_chunks);

  blake3_hasher_init_derive_key(&hasher, RPC_ROOT_CONTEXT);
  blake3_hasher_update(&hasher, header, sizeof(header));
  for (i = 0u; i < total_chunks; ++i) {
    rpc_store32_le(record_header, i);
    rpc_store64_le(record_header + 4u, chunk_lengths[i]);
    blake3_hasher_update(&hasher, record_header, sizeof(record_header));
    blake3_hasher_update(&hasher,
                         chunk_digests + ((size_t)i * RPC_DIGEST_LEN),
                         RPC_DIGEST_LEN);
  }
  blake3_hasher_finalize(&hasher, out, RPC_DIGEST_LEN);
  return RPC_OK;
}

int rpc_digest_equal(const uint8_t left[RPC_DIGEST_LEN],
                     const uint8_t right[RPC_DIGEST_LEN]) {
  uint8_t diff = 0u;
  size_t i;

  if (left == NULL || right == NULL) {
    return 0;
  }
  for (i = 0u; i < RPC_DIGEST_LEN; ++i) {
    diff |= (uint8_t)(left[i] ^ right[i]);
  }
  return diff == 0u;
}

rpc_status rpc_stream_init(rpc_stream_state *state,
                           const uint8_t packet_id[RPC_PACKET_ID_LEN],
                           uint32_t total_chunks) {
  if (state == NULL || packet_id == NULL) {
    return RPC_ERR_ARGUMENT;
  }
  if (!rpc_valid_total(total_chunks)) {
    return RPC_ERR_RANGE;
  }

  rpc_zero(state, sizeof(*state));
  rpc_copy(state->packet_id, packet_id, RPC_PACKET_ID_LEN);
  state->total_chunks = total_chunks;
  return RPC_OK;
}

rpc_status rpc_stream_record(rpc_stream_state *state,
                             uint32_t chunk_index,
                             const void *payload,
                             size_t payload_len,
                             const uint8_t expected_digest[RPC_DIGEST_LEN]) {
  uint8_t actual[RPC_DIGEST_LEN];
  rpc_status status;

  if (state == NULL || expected_digest == NULL ||
      (payload == NULL && payload_len != 0u)) {
    return RPC_ERR_ARGUMENT;
  }
  if (!rpc_valid_total(state->total_chunks) ||
      chunk_index >= state->total_chunks) {
    return RPC_ERR_RANGE;
  }

  status = rpc_chunk_digest(state->packet_id,
                            chunk_index,
                            state->total_chunks,
                            payload,
                            payload_len,
                            actual);
  if (status != RPC_OK) {
    return status;
  }
  if (!rpc_digest_equal(actual, expected_digest)) {
    state->rejected_chunks += 1u;
    return RPC_ERR_DIGEST;
  }
  if (rpc_bit_is_set(state, chunk_index)) {
    return RPC_DUPLICATE;
  }

  rpc_bit_set(state, chunk_index);
  state->received_chunks += 1u;
  state->verified_bytes += (uint64_t)payload_len;
  return RPC_OK;
}

rpc_status rpc_stream_next_missing(const rpc_stream_state *state,
                                   uint32_t start_index,
                                   uint32_t *missing_index) {
  uint32_t i;

  if (state == NULL || missing_index == NULL) {
    return RPC_ERR_ARGUMENT;
  }
  if (!rpc_valid_total(state->total_chunks) ||
      start_index > state->total_chunks) {
    return RPC_ERR_RANGE;
  }

  for (i = start_index; i < state->total_chunks; ++i) {
    if (!rpc_bit_is_set(state, i)) {
      *missing_index = i;
      return RPC_OK;
    }
  }
  return RPC_INCOMPLETE;
}

int rpc_stream_complete(const rpc_stream_state *state) {
  if (state == NULL || !rpc_valid_total(state->total_chunks)) {
    return 0;
  }
  return state->received_chunks == state->total_chunks;
}
