/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Pure integer validation gate. No libc, heap, I/O, float or libm.
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

/*
 * alpha_ok is computed by the versioned numeric adapter.
 * This core intentionally receives the discrete gate rather than floating
 * point so freestanding targets do not acquire hidden soft-float helpers.
 */
int pai_validate_core_check(int alpha_ok,
                            int attractors,
                            int rows,
                            int cols,
                            int dr,
                            int dc,
                            int length,
                            pai_validate_core_result *out);

#endif
