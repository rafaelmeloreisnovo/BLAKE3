// SPDX-License-Identifier: Apache-2.0 OR CC0-1.0
#define _POSIX_C_SOURCE 200809L
#include "rmr_stability.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COLUMNS 128

typedef struct {
    char *name[MAX_COLUMNS];
    int count;
} Header;

static int split_csv(char *line, char **cells, int max_cells) {
    int count = 0;
    char *p = line;
    while (*p && count < max_cells) {
        cells[count++] = p;
        while (*p && *p != ',' && *p != '\r' && *p != '\n') ++p;
        if (*p == ',') *p++ = '\0';
        else { *p = '\0'; break; }
    }
    return count;
}

static int find_column(const Header *header, const char *name) {
    for (int i = 0; i < header->count; ++i) {
        if (strcmp(header->name[i], name) == 0) return i;
    }
    return -1;
}

static int parse_u32(const char *text, uint32_t *value) {
    if (!text || !*text || !value) return 0;
    errno = 0;
    char *end = NULL;
    const unsigned long parsed = strtoul(text, &end, 10);
    if (errno != 0 || end == text || (*end != '\0' && *end != '\r' && *end != '\n') || parsed > 0xFFFFFFFFul) return 0;
    *value = (uint32_t)parsed;
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s triad_trace.csv\n", argv[0]);
        return 2;
    }
    FILE *input = fopen(argv[1], "rb");
    if (!input) { perror(argv[1]); return 1; }

    char *line = NULL;
    size_t capacity = 0u;
    if (getline(&line, &capacity, input) < 0) {
        fprintf(stderr, "empty trace\n");
        fclose(input); free(line); return 1;
    }

    Header header = {{0},0};
    header.count = split_csv(line, header.name, MAX_COLUMNS);
    const int i_stable = find_column(&header, "stable_any");
    const int i_peak = find_column(&header, "gate_in_peaks");
    int i_gate = find_column(&header, "gate_9to1");
    if (i_gate < 0) i_gate = find_column(&header, "gate");
    if (i_stable < 0 || (i_peak < 0 && i_gate < 0)) {
        fprintf(stderr, "required columns: stable_any and gate_in_peaks or gate_9to1/gate\n");
        fclose(input); free(line); return 1;
    }

    RmR_StabilityTrace trace;
    RmR_StabilityTrace_Init(&trace);
    uint32_t malformed = 0u;

    while (getline(&line, &capacity, input) >= 0) {
        char *cells[MAX_COLUMNS] = {0};
        const int cell_count = split_csv(line, cells, MAX_COLUMNS);
        if (cell_count <= i_stable || (i_peak >= 0 && cell_count <= i_peak) || (i_gate >= 0 && cell_count <= i_gate)) {
            ++malformed;
            continue;
        }
        uint32_t stable = 0u, peak = 0u, gate = 0u;
        if (!parse_u32(cells[i_stable], &stable)) { ++malformed; continue; }
        if (i_peak >= 0 && !parse_u32(cells[i_peak], &peak)) { ++malformed; continue; }
        if (i_gate >= 0 && !parse_u32(cells[i_gate], &gate)) { ++malformed; continue; }
        RmR_StabilityTrace_Add(&trace, gate, (uint8_t)(stable != 0u), (uint8_t)(peak != 0u));
    }

    fclose(input);
    free(line);
    const uint32_t status = RmR_StabilityTrace_Finalize(&trace);
    const double peak_rate = (double)trace.peak_rate_q30 / (double)RMR_STABILITY_Q30_ONE;
    const double nonpeak_rate = (double)trace.nonpeak_rate_q30 / (double)RMR_STABILITY_Q30_ONE;
    const double delta_p = (double)trace.delta_p_q30 / (double)RMR_STABILITY_Q30_ONE;

    printf("{\n");
    printf("  \"metric\": \"delta_p_association\",\n");
    printf("  \"meaning\": \"P(stable|peaks)-P(stable|nonpeaks)\",\n");
    printf("  \"rows\": %u,\n", trace.rows);
    printf("  \"malformed_rows\": %u,\n", malformed);
    printf("  \"peak_total\": %u,\n", trace.peak_total);
    printf("  \"peak_stable\": %u,\n", trace.peak_stable);
    printf("  \"nonpeak_total\": %u,\n", trace.nonpeak_total);
    printf("  \"nonpeak_stable\": %u,\n", trace.nonpeak_stable);
    printf("  \"peak_rate\": %.9f,\n", peak_rate);
    printf("  \"nonpeak_rate\": %.9f,\n", nonpeak_rate);
    printf("  \"delta_p\": %.9f,\n", delta_p);
    printf("  \"delta_p_q30\": %d,\n", trace.delta_p_q30);
    printf("  \"status_flags\": %u,\n", status);
    printf("  \"claim_boundary\": \"measured association; not semantic accuracy or causality\"\n");
    printf("}\n");
    return (status == RMR_STABILITY_STATUS_OK && malformed == 0u) ? 0 : 3;
}
