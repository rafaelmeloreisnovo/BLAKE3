/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#include "../include/rmr_pai42.h"

static const int32_t RMR_PAI42_COS_Q16[RMR_PAI42_COUNT] = {
    65536, 64804, 62624, 59046, 54148, 48041, 40861,
    32768, 23943, 14583, 4898, -4898, -14583, -23943,
    -32768, -40861, -48041, -54148, -59046, -62624, -64804,
    -65536, -64804, -62624, -59046, -54148, -48041, -40861,
    -32768, -23943, -14583, -4898, 4898, 14583, 23943,
    32768, 40861, 48041, 54148, 59046, 62624, 64804
};

static const int32_t RMR_PAI42_SIN_Q16[RMR_PAI42_COUNT] = {
    0, 9768, 19317, 28435, 36918, 44576, 51238,
    56756, 61006, 63893, 65353, 65353, 63893, 61006,
    56756, 51238, 44576, 36918, 28435, 19317, 9768,
    0, -9768, -19317, -28435, -36918, -44576, -51238,
    -56756, -61006, -63893, -65353, -65353, -63893, -61006,
    -56756, -51238, -44576, -36918, -28435, -19317, -9768
};

static uint32_t rmr_pai42_abs_diff_u32(uint32_t a, uint32_t b) {
    return a >= b ? a - b : b - a;
}

static uint32_t rmr_pai42_clamp_q16(uint64_t value) {
    return value > RMR_PAI42_Q16_ONE ? RMR_PAI42_Q16_ONE : (uint32_t)value;
}

int rmr_pai42_build(uint64_t hw_sig64,
                    const uint64_t cycles[RMR_PAI42_COUNT],
                    rmr_pai42_observation *out) {
    uint64_t min_cycle;
    uint64_t max_cycle;
    uint64_t span;
    uint64_t sum_radius = 0u;
    uint64_t sum_squared = 0u;
    uint64_t sum_opposite = 0u;
    uint32_t mean_radius;
    uint32_t i;

    if (!cycles || !out) return -1;

    min_cycle = cycles[0];
    max_cycle = cycles[0];
    for (i = 1u; i < RMR_PAI42_COUNT; ++i) {
        if (cycles[i] < min_cycle) min_cycle = cycles[i];
        if (cycles[i] > max_cycle) max_cycle = cycles[i];
    }
    span = max_cycle - min_cycle;

    out->version = RMR_PAI42_VERSION;
    out->count = RMR_PAI42_COUNT;
    out->hw_sig64 = hw_sig64;
    out->min_cycle = min_cycle;
    out->max_cycle = max_cycle;

    for (i = 0u; i < RMR_PAI42_COUNT; ++i) {
        uint32_t radius;
        if (span == 0u) {
            radius = RMR_PAI42_Q16_ONE / 2u;
        } else {
            uint64_t delta = cycles[i] - min_cycle;
            radius = (uint32_t)((delta * RMR_PAI42_Q16_ONE) / span);
        }
        out->radius_q16[i] = radius;
        out->x_q16[i] = (int32_t)(((int64_t)RMR_PAI42_COS_Q16[i] * radius) >> 16);
        out->y_q16[i] = (int32_t)(((int64_t)RMR_PAI42_SIN_Q16[i] * radius) >> 16);
        sum_radius += radius;
    }

    mean_radius = (uint32_t)(sum_radius / RMR_PAI42_COUNT);
    out->mean_radius_q16 = mean_radius;

    for (i = 0u; i < RMR_PAI42_COUNT; ++i) {
        uint32_t deviation = rmr_pai42_abs_diff_u32(out->radius_q16[i], mean_radius);
        sum_squared += (uint64_t)deviation * deviation;
    }

    for (i = 0u; i < RMR_PAI42_COUNT / 2u; ++i) {
        sum_opposite += rmr_pai42_abs_diff_u32(
            out->radius_q16[i], out->radius_q16[i + RMR_PAI42_COUNT / 2u]);
    }

    out->variance_q16 = rmr_pai42_clamp_q16(
        sum_squared / ((uint64_t)RMR_PAI42_COUNT * RMR_PAI42_Q16_ONE));
    out->symmetry_q16 = RMR_PAI42_Q16_ONE - rmr_pai42_clamp_q16(
        sum_opposite / (RMR_PAI42_COUNT / 2u));
    out->stability_q16 = RMR_PAI42_Q16_ONE - out->variance_q16;
    out->prototype_score_q16 = (uint32_t)(
        ((uint64_t)55u * out->symmetry_q16 +
         (uint64_t)45u * out->stability_q16) / 100u);

    return rmr_pai42_validate(out) ? 0 : -2;
}

int rmr_pai42_validate(const rmr_pai42_observation *observation) {
    uint32_t i;
    if (!observation) return 0;
    if (observation->version != RMR_PAI42_VERSION) return 0;
    if (observation->count != RMR_PAI42_COUNT) return 0;
    if (observation->min_cycle > observation->max_cycle) return 0;
    if (observation->mean_radius_q16 > RMR_PAI42_Q16_ONE) return 0;
    if (observation->variance_q16 > RMR_PAI42_Q16_ONE) return 0;
    if (observation->symmetry_q16 > RMR_PAI42_Q16_ONE) return 0;
    if (observation->stability_q16 > RMR_PAI42_Q16_ONE) return 0;
    if (observation->prototype_score_q16 > RMR_PAI42_Q16_ONE) return 0;
    for (i = 0u; i < RMR_PAI42_COUNT; ++i) {
        if (observation->radius_q16[i] > RMR_PAI42_Q16_ONE) return 0;
    }
    return 1;
}
