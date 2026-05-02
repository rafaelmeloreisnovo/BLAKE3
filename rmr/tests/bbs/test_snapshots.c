/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
#include "../../ui/bbs/bbs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int load_file(const char *path, char *out, size_t n) {
    FILE *fp = fopen(path, "r");
    if(!fp) return 1;
    size_t r = fread(out, 1, n - 1, fp);
    out[r] = '\0';
    fclose(fp);
    return 0;
}

int main(void) {
    const char *items[] = {"Item 1", "Item 2", "Item 3", "Item 4"};
    rmr_bbs_menu_state menu;
    rmr_bbs_theme theme;
    char out[512];
    char got[512];

    if(rmr_bbs_theme_load("rmr/ui/bbs/theme.ini", &theme) != 0) return 2;
    rmr_bbs_menu_init(&menu, items, 4, 2);
    rmr_bbs_menu_apply_key(&menu, RMR_BBS_KEY_DOWN);

    rmr_bbs_render_menu(&menu, &theme, RMR_BBS_TERM_ANSI, out, sizeof(out));
    {
        char escaped[512];
        size_t j = 0;
        for(size_t i = 0; out[i] && j + 4 < sizeof(escaped); i++) {
            if((unsigned char)out[i] == 0x1b) {
                escaped[j++] = '\\'; escaped[j++] = 'x'; escaped[j++] = '1'; escaped[j++] = 'b';
            } else {
                escaped[j++] = out[i];
            }
        }
        escaped[j] = '\0';
        strcpy(out, escaped);
    }
    if(load_file("rmr/tests/bbs/golden/menu_ansi.txt", got, sizeof(got)) != 0) return 3;
    if(strcmp(out, got) != 0) {
        fprintf(stderr, "ansi snapshot mismatch\n--- got ---\n%s--- exp ---\n%s", out, got);
        return 4;
    }

    rmr_bbs_render_menu(&menu, &theme, RMR_BBS_TERM_ASCII, out, sizeof(out));
    if(load_file("rmr/tests/bbs/golden/menu_ascii.txt", got, sizeof(got)) != 0) return 5;
    if(strcmp(out, got) != 0) {
        fprintf(stderr, "ascii snapshot mismatch\n--- got ---\n%s--- exp ---\n%s", out, got);
        return 6;
    }
    puts("ok");
    return 0;
}
