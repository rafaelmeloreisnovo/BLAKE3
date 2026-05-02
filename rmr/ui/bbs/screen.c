/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "bbs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

rmr_bbs_term_mode rmr_bbs_detect_terminal_mode(void) {
    const char *term = getenv("TERM");
    if(term && strcmp(term, "dumb") != 0) return RMR_BBS_TERM_ANSI;
    return RMR_BBS_TERM_ASCII;
}

int rmr_bbs_theme_load(const char *path, rmr_bbs_theme *out_theme) {
    FILE *fp = fopen(path, "r");
    char line[128];
    if(!out_theme) return 1;
    strncpy(out_theme->active_prefix, "> ", sizeof(out_theme->active_prefix) - 1);
    strncpy(out_theme->inactive_prefix, "  ", sizeof(out_theme->inactive_prefix) - 1);
    out_theme->active_suffix[0] = '\0';

    if(!fp) return 1;
    while(fgets(line, sizeof(line), fp)) {
        char *eq = strchr(line, '=');
        if(!eq) continue;
        *eq = '\0';
        char *k = line;
        char *v = eq + 1;
        v[strcspn(v, "\r\n")] = '\0';
        if(strcmp(k, "active_prefix") == 0) strncpy(out_theme->active_prefix, v, sizeof(out_theme->active_prefix) - 1);
        if(strcmp(k, "inactive_prefix") == 0) strncpy(out_theme->inactive_prefix, v, sizeof(out_theme->inactive_prefix) - 1);
        if(strcmp(k, "active_suffix") == 0) strncpy(out_theme->active_suffix, v, sizeof(out_theme->active_suffix) - 1);
    }
    fclose(fp);
    return 0;
}

void rmr_bbs_render_menu(const rmr_bbs_menu_state *state,
                         const rmr_bbs_theme *theme,
                         rmr_bbs_term_mode mode,
                         char *out,
                         size_t out_size) {
    size_t used = 0;
    int last = state->top_index + state->page_size;
    if(last > state->item_count) last = state->item_count;

    used += (size_t)snprintf(out + used, out_size - used, "+---------------- BBS ----------------+\n");
    for(int i = state->top_index; i < last; i++) {
        const int active = (i == state->selected);
        const char *prefix = active ? theme->active_prefix : theme->inactive_prefix;
        const char *suffix = active ? theme->active_suffix : "";
        if(mode == RMR_BBS_TERM_ANSI && active) {
            used += (size_t)snprintf(out + used, out_size - used, "\x1b[7m%s%s%s\x1b[0m\n", prefix, state->items[i], suffix);
        } else {
            used += (size_t)snprintf(out + used, out_size - used, "%s%s%s\n", prefix, state->items[i], suffix);
        }
    }
    used += (size_t)snprintf(out + used, out_size - used,
                             "[Pg %d/%d] Use Up/Down, Enter=OK, Esc=Voltar\n",
                             (state->top_index / state->page_size) + 1,
                             (state->item_count + state->page_size - 1) / state->page_size);
}
