/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum rmr_bbs_key {
    RMR_BBS_KEY_NONE = 0,
    RMR_BBS_KEY_UP,
    RMR_BBS_KEY_DOWN,
    RMR_BBS_KEY_ENTER,
    RMR_BBS_KEY_ESC
} rmr_bbs_key;

typedef enum rmr_bbs_term_mode {
    RMR_BBS_TERM_ASCII = 0,
    RMR_BBS_TERM_ANSI = 1
} rmr_bbs_term_mode;

typedef struct rmr_bbs_theme {
    char active_prefix[16];
    char inactive_prefix[16];
    char active_suffix[16];
} rmr_bbs_theme;

typedef struct rmr_bbs_menu_state {
    const char **items;
    int item_count;
    int selected;
    int page_size;
    int top_index;
} rmr_bbs_menu_state;

rmr_bbs_term_mode rmr_bbs_detect_terminal_mode(void);
int rmr_bbs_theme_load(const char *path, rmr_bbs_theme *out_theme);
rmr_bbs_key rmr_bbs_decode_input(const char *seq, size_t len);
void rmr_bbs_menu_init(rmr_bbs_menu_state *state, const char **items, int item_count, int page_size);
void rmr_bbs_menu_apply_key(rmr_bbs_menu_state *state, rmr_bbs_key key);
void rmr_bbs_render_menu(const rmr_bbs_menu_state *state,
                         const rmr_bbs_theme *theme,
                         rmr_bbs_term_mode mode,
                         char *out,
                         size_t out_size);

#ifdef __cplusplus
}
#endif
