#define _POSIX_C_SOURCE 200809L

#include "blake3.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE (1024u * 1024u)

static uint8_t input_buffer[BUFFER_SIZE];

static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end) {
  return (double)(end->tv_sec - start->tv_sec) +
         (double)(end->tv_nsec - start->tv_nsec) / 1000000000.0;
}

int main(int argc, char **argv) {
  const char *mode = argc > 1 ? argv[1] : "unknown";
  unsigned long iterations = 512;

  if (argc > 2) {
    iterations = strtoul(argv[2], NULL, 10);
    if (iterations == 0) {
      fprintf(stderr, "iterations must be greater than zero\n");
      return 2;
    }
  }

  uint32_t state = UINT32_C(0x9e3779b9);
  for (size_t i = 0; i < BUFFER_SIZE; ++i) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    input_buffer[i] = (uint8_t)state;
  }

  uint8_t digest[BLAKE3_OUT_LEN] = {0};
  uint32_t guard = 0;

  for (unsigned int warmup = 0; warmup < 8; ++warmup) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, input_buffer, BUFFER_SIZE);
    blake3_hasher_finalize(&hasher, digest, sizeof(digest));
    guard ^= digest[warmup];
  }

  struct timespec begin;
  struct timespec end;

  if (clock_gettime(CLOCK_MONOTONIC, &begin) != 0) {
    perror("clock_gettime");
    return 3;
  }

  for (unsigned long iteration = 0; iteration < iterations; ++iteration) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, input_buffer, BUFFER_SIZE);
    blake3_hasher_finalize(&hasher, digest, sizeof(digest));
    guard ^= digest[iteration % BLAKE3_OUT_LEN];
  }

  if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
    perror("clock_gettime");
    return 4;
  }

  const double seconds = elapsed_seconds(&begin, &end);
  const double mib_per_second = (double)iterations / seconds;

  printf("RESULT,%s,%.9f,%.3f,%08x,", mode, seconds, mib_per_second,
         guard);
  for (size_t i = 0; i < sizeof(digest); ++i) {
    printf("%02x", digest[i]);
  }
  putchar('\n');

  return 0;
}
