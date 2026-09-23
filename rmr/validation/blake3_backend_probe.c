/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
#include "blake3_impl.h"
#include <stdio.h>

int main(void) {
  printf("simd_degree=%zu\n", blake3_simd_degree());
  return 0;
}
