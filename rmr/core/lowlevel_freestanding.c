/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "../include/rmr_lowlevel.h"

#if defined(RMR_NO_LIBC) && defined(RMR_FREESTANDING_NOMALLOC)
uint8_t rmr_freestanding_arena[RMR_FREESTANDING_ARENA_SIZE];
size_t rmr_freestanding_arena_head;
#endif
