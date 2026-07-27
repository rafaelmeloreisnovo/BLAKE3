/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#ifndef RMR_PAI42_H
#define RMR_PAI42_H

#include <stdint.h>

#define RMR_PAI42_COUNT 42u
#define RMR_PAI42_Q16_ONE 65536u
#define RMR_PAI42_VERSION 1u

/*
 * Fixed-size observation derived from exactly 42 ATA OMEGA cycle samples.
 * No heap, no floating point and no external dependency are required.
 */
typedef struct rmr_pai42_observation {
    uint32_t version;
    uint32_t count;
    uint64_t hw_sig64;
    uint64_t min_cycle;
    uint64_t max_cycle;
    uint32_t mean_radius_q16;
    uint32_t variance_q16;
    uint32_t symmetry_q16;
    uint32_t stability_q16;
    uint32_t prototype_score_q16;
    uint32_t radius_q16[RMR_PAI42_COUNT];
    int32_t x_q16[RMR_PAI42_COUNT];
    int32_t y_q16[RMR_PAI42_COUNT];
} rmr_pai42_observation;

/*
 * Builds a deterministic 42-point circular projection.
 * Returns 0 on success and a negative value on invalid input.
 */
int rmr_pai42_build(uint64_t hw_sig64,
                    const uint64_t cycles[RMR_PAI42_COUNT],
                    rmr_pai42_observation *out);

/* Returns 1 when the structural invariants hold, otherwise 0. */
int rmr_pai42_validate(const rmr_pai42_observation *observation);

#endif
