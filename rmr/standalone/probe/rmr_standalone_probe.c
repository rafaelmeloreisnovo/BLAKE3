/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * Link witness only. Deliberately performs no syscall and never returns.
 */
#include "../include/rmr_standalone.h"

static volatile rmr_sa_u32 rmr_sa_probe_sink;

RMR_SA_USED RMR_SA_NOINLINE
void rmr_sa_probe_entry(void) {
    rmr_sa_u8 source[32];
    rmr_sa_u8 target[32];
    rmr_sa_q_profile q;
    rmr_sa_u32 state = RMR_SA_OK;

    for (rmr_sa_u32 i = 0u; i < 32u; ++i) {
        source[i] = (rmr_sa_u8)(i * 7u + 3u);
    }

    rmr_sa_zero(target, (rmr_sa_usize)32u);
    rmr_sa_copy(target, source, (rmr_sa_usize)32u);

    if (rmr_sa_equal(source, target, (rmr_sa_usize)32u) == 0u) {
        state |= 0x100u;
    }
    if (rmr_sa_q_profile_get(RMR_SA_Q8_FRAC, &q) != RMR_SA_OK) {
        state |= 0x200u;
    }
    if (rmr_sa_q_profile_get(RMR_SA_Q16_FRAC, &q) != RMR_SA_OK) {
        state |= 0x400u;
    }
    if (rmr_sa_q_profile_get(RMR_SA_Q32_FRAC, &q) != RMR_SA_OK) {
        state |= 0x800u;
    }
    if (rmr_sa_q_profile_get(RMR_SA_Q42_FRAC, &q) != RMR_SA_OK) {
        state |= 0x1000u;
    }

    rmr_sa_probe_sink = state ^ rmr_sa_xor_fold32(target, (rmr_sa_usize)32u);

    for (;;) {
        /* Intentional no-syscall terminal state for a link witness. */
    }
}
