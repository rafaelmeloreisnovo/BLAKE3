/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * RMR harmonic-point composition reference implementation.
 * Original algorithmic score only; no third-party melody is embedded.
 * This file does not modify or replace the BLAKE3 core.
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    uint32_t num;
    uint32_t den;
    uint32_t geometry_point;
    int32_t phase_mdeg;
    uint32_t weight_q16;
} rmr_harmonic_point;

/* Mathematical ratios are facts; this ordered selection/score is RMR expression. */
static const rmr_harmonic_point k_points[8] = {
    {1, 1, 0,      0, 65536},
    {9, 8, 1,  45000, 57344},
    {5, 4, 2,  90000, 61440},
    {4, 3, 3, 135000, 53248},
    {3, 2, 4, 180000, 65536},
    {5, 3, 5, 225000, 57344},
    {15,8, 6, 270000, 49152},
    {2, 1, 7, 315000, 61440},
};

/* Deterministic original 16-step orbit; not copied from an existing song. */
static const uint8_t k_score[16] = {
    0,4,2,5,3,6,1,7, 4,0,5,2,6,3,7,1
};

static void put_u16le(FILE *f, uint16_t v) {
    fputc((int)(v & 0xffu), f);
    fputc((int)((v >> 8) & 0xffu), f);
}

static void put_u32le(FILE *f, uint32_t v) {
    fputc((int)(v & 0xffu), f);
    fputc((int)((v >> 8) & 0xffu), f);
    fputc((int)((v >> 16) & 0xffu), f);
    fputc((int)((v >> 24) & 0xffu), f);
}

static double shannon_entropy_bytes(const uint8_t *p, size_t n) {
    uint64_t hist[256] = {0};
    double h = 0.0;
    size_t i;
    if (!p || n == 0) return 0.0;
    for (i = 0; i < n; ++i) hist[p[i]]++;
    for (i = 0; i < 256; ++i) {
        if (hist[i]) {
            const double q = (double)hist[i] / (double)n;
            h -= q * (log(q) / log(2.0));
        }
    }
    return h;
}

static int write_manifest(const char *path, double base_hz, uint32_t sr,
                          uint32_t frames, double entropy) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fprintf(f,
        "{\n"
        "  \"format\": \"RMR_HARMONIC_POINTS_V1\",\n"
        "  \"base_hz\": %.9f,\n"
        "  \"sample_rate\": %u,\n"
        "  \"frames\": %u,\n"
        "  \"point_count\": 8,\n"
        "  \"score_steps\": 16,\n"
        "  \"entropy_bits_per_pcm_byte\": %.9f,\n"
        "  \"entropy_semantics\": \"diagnostic_only_not_quality_or_spiritual_proof\",\n"
        "  \"zip_route\": \"rmr/crypto/ZIP_BITSTACK_CUSTODY_PROFILE.md\",\n"
        "  \"claim_allowed\": false\n"
        "}\n",
        base_hz, sr, frames, entropy);
    return fclose(f) == 0 ? 0 : -1;
}

int main(int argc, char **argv) {
    const char *wav_path = argc > 1 ? argv[1] : "rmr_harmonic_points.wav";
    const char *manifest_path = argc > 2 ? argv[2] : "rmr_harmonic_points.manifest.json";
    const double base_hz = argc > 3 ? strtod(argv[3], NULL) : 220.0;
    const uint32_t sample_rate = 48000;
    const double step_seconds = 0.50;
    const uint32_t step_frames = (uint32_t)(sample_rate * step_seconds);
    const uint32_t total_frames = step_frames * 16u;
    const size_t pcm_bytes = (size_t)total_frames * 2u;
    uint8_t *pcm = (uint8_t *)malloc(pcm_bytes);
    FILE *f;
    uint32_t frame;

    if (!pcm || base_hz <= 0.0) {
        free(pcm);
        return 2;
    }

    for (frame = 0; frame < total_frames; ++frame) {
        const uint32_t step = frame / step_frames;
        const rmr_harmonic_point *pt = &k_points[k_score[step]];
        const double local_t = (double)(frame % step_frames) / (double)sample_rate;
        const double f0 = base_hz * (double)pt->num / (double)pt->den;
        const double phase = ((double)pt->phase_mdeg / 1000.0) * M_PI / 180.0;
        const double w = (double)pt->weight_q16 / 65536.0;
        const double attack = local_t < 0.04 ? local_t / 0.04 : 1.0;
        const double remain = step_seconds - local_t;
        const double release = remain < 0.08 ? remain / 0.08 : 1.0;
        const double env = attack * release;
        /* Fundamental + fifth-related overtone. Deliberately bounded. */
        double s = sin(2.0 * M_PI * f0 * local_t + phase);
        s += 0.35 * sin(2.0 * M_PI * f0 * 1.5 * local_t + phase * 0.5);
        s *= 0.42 * w * env;
        if (s > 1.0) s = 1.0;
        if (s < -1.0) s = -1.0;
        {
            const int16_t v = (int16_t)lrint(s * 32767.0);
            const size_t off = (size_t)frame * 2u;
            pcm[off] = (uint8_t)((uint16_t)v & 0xffu);
            pcm[off + 1u] = (uint8_t)(((uint16_t)v >> 8) & 0xffu);
        }
    }

    f = fopen(wav_path, "wb");
    if (!f) { free(pcm); return 3; }
    fwrite("RIFF", 1, 4, f);
    put_u32le(f, 36u + (uint32_t)pcm_bytes);
    fwrite("WAVEfmt ", 1, 8, f);
    put_u32le(f, 16u);
    put_u16le(f, 1u);
    put_u16le(f, 1u);
    put_u32le(f, sample_rate);
    put_u32le(f, sample_rate * 2u);
    put_u16le(f, 2u);
    put_u16le(f, 16u);
    fwrite("data", 1, 4, f);
    put_u32le(f, (uint32_t)pcm_bytes);
    if (fwrite(pcm, 1, pcm_bytes, f) != pcm_bytes || fclose(f) != 0) {
        free(pcm);
        return 4;
    }

    {
        const double h = shannon_entropy_bytes(pcm, pcm_bytes);
        fprintf(stderr, "RMR harmonic points: entropy=%.9f bits/PCM-byte; diagnostic only\n", h);
        if (write_manifest(manifest_path, base_hz, sample_rate, total_frames, h) != 0) {
            free(pcm);
            return 5;
        }
    }

    free(pcm);
    return 0;
}
