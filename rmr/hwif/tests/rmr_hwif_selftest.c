/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
#include "../include/rmr_hwif.h"

#include <stdint.h>
#include <string.h>

int main(void) {
    const rmr_hwif_ops *ops = rmr_hwif_current();

    if (sizeof(rmr_u64) != 8u || sizeof(rmr_s64) != 8u) {
        return 1;
    }
    if (ops == (const rmr_hwif_ops *)0 || ops->backend_name == (const char *)0) {
        return 2;
    }

#if defined(RMR_ARCH_X86_64)
    if (strcmp(ops->backend_name, "x86_64_asm") != 0) {
        return 3;
    }
    if ((ops->capability_flags & RMR_HWIF_CAP_TIME_HARDWARE) == 0u) {
        return 4;
    }
    if ((ops->capability_flags & RMR_HWIF_CAP_CPU_ID_HARDWARE) == 0u) {
        return 5;
    }
#endif

#if defined(RMR_ARCH_ARM) && defined(RMR_OS_LINUX) && !defined(RMR_ARMV7_ASSUME_PRIVILEGED)
    if (strcmp(ops->backend_name, "armv7_linux_user") != 0) {
        return 6;
    }
    if ((ops->capability_flags & RMR_HWIF_CAP_PRIVILEGED_REGS) != 0u) {
        return 7;
    }
#endif

    return 0;
}
