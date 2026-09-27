/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * Hosted test harness; production core remains freestanding.
 */
#include "../include/rmr_standalone.h"

int main(void) {
    rmr_sa_u8 a[64];
    rmr_sa_u8 b[64];
    rmr_sa_q_profile q;

    for (rmr_sa_u32 i = 0u; i < 64u; ++i) {
        a[i] = (rmr_sa_u8)(i ^ 0x5au);
    }

    rmr_sa_zero(b, (rmr_sa_usize)64u);
    if (rmr_sa_equal(a, b, (rmr_sa_usize)64u) != 0u) {
        return 1;
    }

    rmr_sa_copy(b, a, (rmr_sa_usize)64u);
    if (rmr_sa_equal(a, b, (rmr_sa_usize)64u) == 0u) {
        return 2;
    }

    b[17] ^= 1u;
    if (rmr_sa_equal(a, b, (rmr_sa_usize)64u) != 0u) {
        return 3;
    }

    if (rmr_sa_q_profile_get(RMR_SA_Q8_FRAC, &q) != RMR_SA_OK ||
        q.storage_bits != 32u) {
        return 4;
    }
    if (rmr_sa_q_profile_get(RMR_SA_Q16_FRAC, &q) != RMR_SA_OK ||
        q.storage_bits != 32u) {
        return 5;
    }
    if (rmr_sa_q_profile_get(RMR_SA_Q32_FRAC, &q) != RMR_SA_OK ||
        q.storage_bits != 64u) {
        return 6;
    }
    if (rmr_sa_q_profile_get(RMR_SA_Q42_FRAC, &q) != RMR_SA_OK ||
        q.storage_bits != 64u) {
        return 7;
    }
    if (rmr_sa_q_profile_get(7u, &q) != RMR_SA_ERR_UNSUPPORTED_Q) {
        return 8;
    }

    return 0;
}
