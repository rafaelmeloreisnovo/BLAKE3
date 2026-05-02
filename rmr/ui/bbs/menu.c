/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "bbs.h"

static void clamp_menu(rmr_bbs_menu_state *state) {
    if(state->item_count <= 0) {
        state->selected = 0;
        state->top_index = 0;
        return;
    }
    if(state->selected < 0) state->selected = 0;
    if(state->selected >= state->item_count) state->selected = state->item_count - 1;

    if(state->page_size <= 0) state->page_size = 5;
    if(state->top_index > state->selected) state->top_index = state->selected;
    if(state->selected >= state->top_index + state->page_size) {
        state->top_index = state->selected - state->page_size + 1;
    }
    if(state->top_index < 0) state->top_index = 0;
}

void rmr_bbs_menu_init(rmr_bbs_menu_state *state, const char **items, int item_count, int page_size) {
    state->items = items;
    state->item_count = item_count;
    state->selected = 0;
    state->page_size = page_size;
    state->top_index = 0;
    clamp_menu(state);
}

void rmr_bbs_menu_apply_key(rmr_bbs_menu_state *state, rmr_bbs_key key) {
    if(key == RMR_BBS_KEY_UP) state->selected--;
    if(key == RMR_BBS_KEY_DOWN) state->selected++;
    clamp_menu(state);
}
