// SPDX-License-Identifier: Apache-2.0 OR CC0-1.0
#include "rmr_stability.h"

#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL:%d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

static int test_state(void) {
    static const uint8_t events[] = {0u,1u,1u,2u,3u,8u,4u,3u};
    RmR_StabilityState a, b;
    RmR_StabilityState_Init(&a, 0x963u);
    RmR_StabilityState_Init(&b, 0x963u);
    const uint32_t sa = RmR_StabilityStepPortable(&a, events, sizeof(events));
    const uint32_t sb = RmR_StabilityStep(&b, events, sizeof(events));
    CHECK(sa == sb);
    CHECK(memcmp(&a, &b, sizeof(a)) == 0);
    CHECK(sa == 1135092322u);
    return 0;
}

static int test_trace(void) {
    RmR_StabilityTrace t;
    RmR_StabilityTrace_Init(&t);
    for (unsigned i = 0; i < 10; ++i) RmR_StabilityTrace_Add(&t, 3u, (uint8_t)(i < 8u), 1u);
    for (unsigned i = 0; i < 10; ++i) RmR_StabilityTrace_Add(&t, 2u, (uint8_t)(i < 2u), 0u);
    CHECK(RmR_StabilityTrace_Finalize(&t) == RMR_STABILITY_STATUS_OK);
    CHECK(t.peak_rate_q30 == 858993459u);
    CHECK(t.nonpeak_rate_q30 == 214748364u);
    CHECK(t.delta_p_q30 == 644245095);

    RmR_StabilityTrace empty;
    RmR_StabilityTrace_Init(&empty);
    const uint32_t status = RmR_StabilityTrace_Finalize(&empty);
    CHECK(status & RMR_STABILITY_STATUS_NO_SAMPLES);
    CHECK(status & RMR_STABILITY_STATUS_NO_PEAK_SAMPLES);
    CHECK(status & RMR_STABILITY_STATUS_NO_NONPEAK_SAMPLES);
    return 0;
}

static int test_vision(void) {
    uint8_t gray[16] = {
        0,0,0,0, 0,0,0,0,
        255,255,255,255, 255,255,255,255
    };
    const int16_t uniform_angles[8] = {0,45,90,135,180,225,270,315};
    const int16_t concentrated_angles[4] = {45,45,45,45};
    RmR_VisionDescriptor uniform_a, concentrated, uniform_b;
    CHECK(RmR_Vision_BuildDescriptor(gray, 4, 4, 4, uniform_angles, 8, &uniform_a) == 0u);
    CHECK(RmR_Vision_BuildDescriptor(gray, 4, 4, 4, concentrated_angles, 4, &concentrated) == 0u);
    CHECK(RmR_Vision_BuildDescriptor(gray, 4, 4, 4, uniform_angles, 8, &uniform_b) == 0u);
    CHECK(uniform_a.angular_chi2_q16 == 0u);
    CHECK(uniform_a.angular_concentration_q16 == 0u);
    CHECK(concentrated.angular_chi2_q16 > 0u);
    CHECK(concentrated.angular_concentration_q16 == RMR_STABILITY_Q16_ONE);
    CHECK(uniform_a.descriptor_crc32c == uniform_b.descriptor_crc32c);
    CHECK(RmR_Vision_DifferenceQ16(&uniform_a, &uniform_b) == 0u);
    CHECK(RmR_Vision_DifferenceQ16(&uniform_a, &concentrated) > 0u);
    CHECK(RmR_Stability_DifferenceHash(&uniform_a, &uniform_b) !=
          RmR_Stability_DifferenceHash(&uniform_a, &concentrated));
    return 0;
}

int main(void) {
    if (test_state()) return 1;
    if (test_trace()) return 1;
    if (test_vision()) return 1;
    puts("rmr_stability_selftest: OK");
    return 0;
}
