/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#include <stdint.h>
#include <stdio.h>
#include "../include/rmr_pai42.h"

static int test_ramp(void) {
    uint64_t cycles[RMR_PAI42_COUNT];
    rmr_pai42_observation observation;
    uint32_t i;
    for (i = 0u; i < RMR_PAI42_COUNT; ++i) cycles[i] = 1000u + i * 7u;
    if (rmr_pai42_build(UINT64_C(0x123456789abcdef0), cycles, &observation) != 0) return 1;
    if (observation.radius_q16[0] != 0u) return 2;
    if (observation.radius_q16[RMR_PAI42_COUNT - 1u] != RMR_PAI42_Q16_ONE) return 3;
    if (observation.hw_sig64 != UINT64_C(0x123456789abcdef0)) return 4;
    return 0;
}

static int test_constant(void) {
    uint64_t cycles[RMR_PAI42_COUNT];
    rmr_pai42_observation observation;
    uint32_t i;
    for (i = 0u; i < RMR_PAI42_COUNT; ++i) cycles[i] = 77u;
    if (rmr_pai42_build(9u, cycles, &observation) != 0) return 10;
    for (i = 0u; i < RMR_PAI42_COUNT; ++i) {
        if (observation.radius_q16[i] != RMR_PAI42_Q16_ONE / 2u) return 11;
    }
    if (observation.variance_q16 != 0u) return 12;
    if (observation.symmetry_q16 != RMR_PAI42_Q16_ONE) return 13;
    if (observation.prototype_score_q16 != RMR_PAI42_Q16_ONE) return 14;
    return 0;
}

int main(void) {
    int rc = test_ramp();
    if (rc != 0) return rc;
    rc = test_constant();
    if (rc != 0) return rc;
    puts("OK: RMR PAI42 fixed-point selftest");
    return 0;
}
