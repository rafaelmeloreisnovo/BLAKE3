/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Pure validation gate. No libc, no heap, no I/O and no libm calls.
 */
#ifndef PAI_VALIDATE_CORE_H
#define PAI_VALIDATE_CORE_H

typedef struct pai_validate_core_result {
    int ok;
    int alpha_ok;
    int attractors_ok;
    int gcd_r;
    int gcd_c;
} pai_validate_core_result;

int pai_validate_core_check(float alpha,
                            int attractors,
                            int rows,
                            int cols,
                            int dr,
                            int dc,
                            int length,
                            pai_validate_core_result *out);

#endif
