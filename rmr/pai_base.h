#pragma once
#include <stdint.h>

int pai_cmd_bases(int argc, char **argv);

// util: primos opostos
int64_t pai_prev_prime(int64_t n);
int64_t pai_next_prime(int64_t n);
