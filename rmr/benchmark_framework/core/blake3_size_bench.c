/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Common size-sweep harness. Link this same file against official BLAKE3 and
 * the RMR/fork BLAKE3 build to obtain comparable calibration rows.
 */

#define _POSIX_C_SOURCE 200809L

#include "blake3.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end) {
  return (double)(end->tv_sec - start->tv_sec) +
         (double)(end->tv_nsec - start->tv_nsec) / 1000000000.0;
}

int main(int argc, char **argv) {
  const char *mode;
  size_t size;
  unsigned long iterations;
  uint8_t *buffer;
  uint8_t digest[BLAKE3_OUT_LEN] = {0};
  uint32_t state = UINT32_C(0x9e3779b9);
  uint32_t guard = 0u;
  struct timespec begin;
  struct timespec end;
  double seconds;
  double ns_per_op;
  double mib_per_second;
  size_t i;
  unsigned long iteration;

  if (argc != 4) {
    fputs("usage: blake3_size_bench MODE SIZE_BYTES ITERATIONS\n", stderr);
    return 2;
  }

  mode = argv[1];
  size = (size_t)strtoull(argv[2], NULL, 10);
  iterations = strtoul(argv[3], NULL, 10);

  if (size == 0u || iterations == 0u) {
    fputs("size and iterations must be greater than zero\n", stderr);
    return 2;
  }

  buffer = (uint8_t *)malloc(size);
  if (buffer == NULL) {
    return 3;
  }

  for (i = 0u; i < size; ++i) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    buffer[i] = (uint8_t)state;
  }

  for (i = 0u; i < 4u; ++i) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, buffer, size);
    blake3_hasher_finalize(&hasher, digest, sizeof(digest));
    guard ^= digest[i];
  }

  if (clock_gettime(CLOCK_MONOTONIC, &begin) != 0) {
    free(buffer);
    return 4;
  }

  for (iteration = 0u; iteration < iterations; ++iteration) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, buffer, size);
    blake3_hasher_finalize(&hasher, digest, sizeof(digest));
    guard ^= digest[iteration % BLAKE3_OUT_LEN];
  }

  if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
    free(buffer);
    return 5;
  }

  seconds = elapsed_seconds(&begin, &end);
  ns_per_op = seconds * 1000000000.0 / (double)iterations;
  mib_per_second =
      ((double)size * (double)iterations / (1024.0 * 1024.0)) / seconds;

  printf(
      "SIZE_RESULT,%s,%zu,%lu,%.9f,%.3f,%.3f,%08x,",
      mode,
      size,
      iterations,
      seconds,
      ns_per_op,
      mib_per_second,
      guard);

  for (i = 0u; i < sizeof(digest); ++i) {
    printf("%02x", digest[i]);
  }
  putchar('\n');

  free(buffer);
  return 0;
}
