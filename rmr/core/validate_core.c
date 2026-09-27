/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Pure integer validation gate. No libc, heap, I/O, float or libm.
 */
#include "pai_validate_core.h"

static int gcd_i(int a, int b) {
    while (b != 0) {
        const int t = b;
        b = a % b;
        a = t;
    }
    return (a < 0) ? -a : a;
}

int pai_validate_core_check(int alpha_ok,
                            int attractors,
                            int rows,
                            int cols,
                            int dr,
                            int dc,
                            int length,
                            pai_validate_core_result *out) {
    if (out == (pai_validate_core_result *)0) {
        return 1;
    }

    out->ok = 0;
    out->alpha_ok = (alpha_ok != 0) ? 1 : 0;
    out->attractors_ok = 0;
    out->gcd_r = 0;
    out->gcd_c = 0;

    if (rows <= 0 || cols <= 0 || length <= 1) {
        return 2;
    }

    out->attractors_ok = (attractors == 42) ? 1 : 0;
    out->gcd_r = gcd_i(dr, rows);
    out->gcd_c = gcd_i(dc, cols);

    out->ok =
        out->alpha_ok &&
        out->attractors_ok &&
        out->gcd_r == 1 &&
        out->gcd_c == 1;

    return 0;
}
