/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

 #include "mode_router.h"
#include "bbs/bbs.h"

#include "../core/pai.h"

#include <stdio.h>
#include <string.h>

static void usage(void) {
    puts("rmr run --mode cli [pai args...]");
    puts("rmr run --mode helper");
    puts("rmr run --mode bbs");
}

static int run_cli_mode(int argc, char **argv) {
    return pai_main(argc, argv);
}

static int helper_choose_profile(char *buf, size_t n) {
    puts("[helper] perfis: ram | io | pipeline");
    printf("[helper] selecione perfil: ");
    if(!fgets(buf, (int)n, stdin)) return 1;
    buf[strcspn(buf, "\r\n")] = '\0';
    return 0;
}

static int run_helper_mode(void) {
    char profile[64] = {0};
    char input[256] = {0};
    char output[256] = {0};
    char confirm[16] = {0};
    char metrics_store[256] = {0};
    char session_mode[32] = {0};

    puts("[helper] wizard iniciado");
    if(helper_choose_profile(profile, sizeof(profile)) != 0) return 1;

    printf("[helper] entrada (--file/--size): ");
    if(!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\r\n")] = '\0';

    printf("[helper] saida (--out/--save): ");
    if(!fgets(output, sizeof(output), stdin)) return 1;
    output[strcspn(output, "\r\n")] = '\0';

    printf("[helper] metrics store (--metrics-store): ");
    if(!fgets(metrics_store, sizeof(metrics_store), stdin)) return 1;
    metrics_store[strcspn(metrics_store, "\r\n")] = '\0';

    printf("[helper] sessao (--append/--new-session): ");
    if(!fgets(session_mode, sizeof(session_mode), stdin)) return 1;
    session_mode[strcspn(session_mode, "\r\n")] = '\0';

    puts("[helper] resumo:");
    printf("  perfil: %s\n", profile);
    printf("  input : %s\n", input);
    printf("  saida : %s\n", output);
    printf("  metrics-store: %s\n", metrics_store);
    printf("  session-mode : %s\n", session_mode);

    printf("[helper] confirmar execucao? (y/N): ");
    if(!fgets(confirm, sizeof(confirm), stdin)) return 1;
    if(confirm[0] != 'y' && confirm[0] != 'Y') {
        puts("[helper] cancelado.");
        return 1;
    }

    puts("[helper] confirmado. encaminhe os parametros para pai_main no backend.");
    return 0;
}

static int run_bbs_mode(void) {
    const char *items[] = {
        "CLI backend (pai_main)",
        "Helper wizard (metrics-store)",
        "Sair"
    };
    rmr_bbs_menu_state menu;
    rmr_bbs_theme theme;
    char screen[1024];
    char input[16];
    rmr_bbs_term_mode mode = rmr_bbs_detect_terminal_mode();

    rmr_bbs_menu_init(&menu, items, 3, 2);
    rmr_bbs_theme_load("rmr/ui/bbs/theme.ini", &theme);

    for(;;) {
        rmr_bbs_render_menu(&menu, &theme, mode, screen, sizeof(screen));
        puts(screen);
        if(!fgets(input, sizeof(input), stdin)) return 1;

        rmr_bbs_key key = rmr_bbs_decode_input(input, strlen(input));
        if(input[0] == 'w') key = RMR_BBS_KEY_UP;
        if(input[0] == 's') key = RMR_BBS_KEY_DOWN;
        if(input[0] == 'q') key = RMR_BBS_KEY_ESC;

        if(key == RMR_BBS_KEY_ENTER) {
            if(menu.selected == 0) {
                puts("[bbs] opcao 1 selecionada: execute 'rmr run --mode cli pai bench --metrics-store <path> --append|--new-session ...'");
                return 0;
            }
            if(menu.selected == 1) return run_helper_mode();
            return 0;
        }
        if(key == RMR_BBS_KEY_ESC) {
            puts("[bbs] encerrado.");
            return 0;
        }
        rmr_bbs_menu_apply_key(&menu, key);
    }
}

int rmr_mode_router_main(int argc, char **argv) {
    const char *mode = NULL;

    if(argc < 2 || strcmp(argv[1], "run") != 0) {
        usage();
        return 1;
    }

    for(int i = 2; i < argc; i++) {
        if(strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
            mode = argv[++i];
            break;
        }
    }

    if(!mode) {
        fprintf(stderr, "[erro] --mode ausente\n");
        usage();
        return 1;
    }

    if(strcmp(mode, "cli") == 0) {
        return run_cli_mode(argc - 2, argv + 2);
    }
    if(strcmp(mode, "helper") == 0) {
        return run_helper_mode();
    }
    if(strcmp(mode, "bbs") == 0) {
        return run_bbs_mode();
    }

    fprintf(stderr, "[erro] modo desconhecido: %s\n", mode);
    usage();
    return 1;
}
