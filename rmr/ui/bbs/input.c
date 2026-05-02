/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "bbs.h"

rmr_bbs_key rmr_bbs_decode_input(const char *seq, size_t len) {
    if(!seq || len == 0) return RMR_BBS_KEY_NONE;
    if(len == 1 && seq[0] == 0x1b) return RMR_BBS_KEY_ESC;
    if(len == 1 && (seq[0] == '\r' || seq[0] == '\n')) return RMR_BBS_KEY_ENTER;
    if(len >= 3 && seq[0] == 0x1b && seq[1] == '[') {
        if(seq[2] == 'A') return RMR_BBS_KEY_UP;
        if(seq[2] == 'B') return RMR_BBS_KEY_DOWN;
    }
    return RMR_BBS_KEY_NONE;
}
