/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Pure integer prime-neighbor core.
 * No libc calls, heap, file I/O, clock or operating-system API.
 */
#include "pai_base.h"

#include <limits.h>

static int is_prime_i64(int64_t x) {
    if (x < 2) {
        return 0;
    }
    if ((x % 2) == 0) {
        return x == 2;
    }

    for (int64_t d = 3; d <= x / d; d += 2) {
        if ((x % d) == 0) {
            return 0;
        }
    }
    return 1;
}

int64_t pai_prev_prime(int64_t n) {
    if (n <= 2) {
        return -1;
    }

    for (int64_t x = n - 1; x >= 2; --x) {
        if (is_prime_i64(x)) {
            return x;
        }
    }
    return -1;
}

int64_t pai_next_prime(int64_t n) {
    if (n == INT64_MAX) {
        return -1;
    }

    int64_t x = (n < 2) ? 2 : n + 1;
    for (;;) {
        if (is_prime_i64(x)) {
            return x;
        }
        if (x == INT64_MAX) {
            return -1;
        }
        ++x;
    }
}
