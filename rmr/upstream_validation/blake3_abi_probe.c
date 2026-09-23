/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
#include "blake3.h"
#include <stddef.h>
#include <stdio.h>

int main(void) {
  printf("version=%s\n", BLAKE3_VERSION_STRING);
  printf("key_len=%d\n", BLAKE3_KEY_LEN);
  printf("out_len=%d\n", BLAKE3_OUT_LEN);
  printf("block_len=%d\n", BLAKE3_BLOCK_LEN);
  printf("chunk_len=%d\n", BLAKE3_CHUNK_LEN);
  printf("sizeof_chunk_state=%zu\n", sizeof(blake3_chunk_state));
  printf("sizeof_hasher=%zu\n", sizeof(blake3_hasher));
  printf("off_hasher_key=%zu\n", offsetof(blake3_hasher,key));
  printf("off_hasher_chunk=%zu\n", offsetof(blake3_hasher,chunk));
  printf("off_hasher_stack_len=%zu\n", offsetof(blake3_hasher,cv_stack_len));
  printf("off_hasher_stack=%zu\n", offsetof(blake3_hasher,cv_stack));
  return 0;
}
