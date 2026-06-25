// BLAKE3 state size probe
//
// Purpose: measure ABI-dependent sizes/offsets for the C state objects.
// This is especially useful for comparing x86-64, AArch64, and ARMv7/Termux.
//
// Example:
//   cc -std=c11 -I../c tools/sizeof_blake3_state.c -o sizeof_blake3_state
//   ./sizeof_blake3_state

#include "blake3.h"
#include <stddef.h>
#include <stdio.h>

int main(void) {
  printf("BLAKE3_OUT_LEN=%d\n", BLAKE3_OUT_LEN);
  printf("BLAKE3_BLOCK_LEN=%d\n", BLAKE3_BLOCK_LEN);
  printf("BLAKE3_CHUNK_LEN=%d\n", BLAKE3_CHUNK_LEN);
  printf("BLAKE3_MAX_DEPTH=%d\n", BLAKE3_MAX_DEPTH);

  printf("sizeof(blake3_chunk_state)=%zu\n", sizeof(blake3_chunk_state));
  printf("offsetof(blake3_chunk_state, cv)=%zu\n", offsetof(blake3_chunk_state, cv));
  printf("offsetof(blake3_chunk_state, chunk_counter)=%zu\n", offsetof(blake3_chunk_state, chunk_counter));
  printf("offsetof(blake3_chunk_state, buf)=%zu\n", offsetof(blake3_chunk_state, buf));
  printf("offsetof(blake3_chunk_state, buf_len)=%zu\n", offsetof(blake3_chunk_state, buf_len));
  printf("offsetof(blake3_chunk_state, blocks_compressed)=%zu\n", offsetof(blake3_chunk_state, blocks_compressed));
  printf("offsetof(blake3_chunk_state, flags)=%zu\n", offsetof(blake3_chunk_state, flags));

  printf("sizeof(blake3_hasher)=%zu\n", sizeof(blake3_hasher));
  printf("offsetof(blake3_hasher, key)=%zu\n", offsetof(blake3_hasher, key));
  printf("offsetof(blake3_hasher, chunk)=%zu\n", offsetof(blake3_hasher, chunk));
  printf("offsetof(blake3_hasher, cv_stack_len)=%zu\n", offsetof(blake3_hasher, cv_stack_len));
  printf("offsetof(blake3_hasher, cv_stack)=%zu\n", offsetof(blake3_hasher, cv_stack));

  printf("computed_cv_stack_bytes=%d\n", (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN);
  printf("blocks_per_chunk=%d\n", BLAKE3_CHUNK_LEN / BLAKE3_BLOCK_LEN);

  return 0;
}
