/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Host-side physical I/O benchmark harness.
 * This is not part of the BLAKE3 primitive or a freestanding core.
 */
#define _GNU_SOURCE 1

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    const char *path;
    const char *mode;
    const char *pattern;
    const char *sync_mode;
    const char *cache_policy;
    uint64_t block_size;
    uint64_t ops;
    uint64_t file_size;
    uint32_t queue_depth;
} rmr_iops_config;

static void usage(const char *argv0) {
    fprintf(stderr,
        "usage: %s --file PATH [--mode read|write] [--pattern seq|random]\\n"
        "          [--block-size N] [--ops N] [--file-size N]\\n"
        "          [--sync none|end|each] [--cache os-default|warm]\\n"
        "          [--queue-depth 1]\\n",
        argv0);
}

static int parse_u64(const char *text, uint64_t *out) {
    char *end = NULL;
    unsigned long long value;

    if (text == NULL || *text == '\0' || out == NULL) {
        return -1;
    }

    errno = 0;
    value = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') {
        return -1;
    }

    *out = (uint64_t)value;
    return 0;
}

static uint64_t now_ns(void) {
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0) {
        return 0u;
    }

    return ((uint64_t)ts.tv_sec * UINT64_C(1000000000)) + (uint64_t)ts.tv_nsec;
}

static uint64_t lcg_next(uint64_t *state) {
    *state = (*state * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407);
    return *state;
}

static const char *arch_name(void) {
#if defined(__x86_64__)
    return "x86_64";
#elif defined(__aarch64__)
    return "aarch64";
#elif defined(__arm__)
    return "arm";
#elif defined(__riscv)
    return "riscv";
#else
    return "unknown";
#endif
}

static int write_full_at(int fd, const void *buf, size_t len, off_t off) {
    const unsigned char *p = (const unsigned char *)buf;
    size_t done = 0u;

    while (done < len) {
        ssize_t n = pwrite(fd, p + done, len - done, off + (off_t)done);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            return -1;
        }
        done += (size_t)n;
    }

    return 0;
}

static int read_full_at(int fd, void *buf, size_t len, off_t off) {
    unsigned char *p = (unsigned char *)buf;
    size_t done = 0u;

    while (done < len) {
        ssize_t n = pread(fd, p + done, len - done, off + (off_t)done);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            return -1;
        }
        done += (size_t)n;
    }

    return 0;
}

static int prepare_file(int fd, unsigned char *buf, const rmr_iops_config *cfg) {
    uint64_t off;

    if (ftruncate(fd, (off_t)cfg->file_size) != 0) {
        return -1;
    }

    memset(buf, 0xA5, (size_t)cfg->block_size);
    for (off = 0u; off < cfg->file_size; off += cfg->block_size) {
        if (write_full_at(fd, buf, (size_t)cfg->block_size, (off_t)off) != 0) {
            return -1;
        }
    }

    return fdatasync(fd);
}

static int warm_file(int fd, unsigned char *buf, const rmr_iops_config *cfg) {
    uint64_t off;

    for (off = 0u; off < cfg->file_size; off += cfg->block_size) {
        if (read_full_at(fd, buf, (size_t)cfg->block_size, (off_t)off) != 0) {
            return -1;
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    rmr_iops_config cfg = {
        NULL,
        "read",
        "seq",
        "none",
        "os-default",
        UINT64_C(4096),
        UINT64_C(4096),
        UINT64_C(16777216),
        1u
    };
    unsigned char *buf = NULL;
    uint64_t start_ns;
    uint64_t end_ns;
    uint64_t elapsed_ns;
    uint64_t completed = 0u;
    uint64_t rng = UINT64_C(0x524D525F494F5053);
    uint64_t slots;
    uint64_t i;
    int fd = -1;
    int rc = 1;

    for (i = 1u; i < (uint64_t)argc; ++i) {
        if (strcmp(argv[i], "--file") == 0 && i + 1u < (uint64_t)argc) {
            cfg.path = argv[++i];
        } else if (strcmp(argv[i], "--mode") == 0 && i + 1u < (uint64_t)argc) {
            cfg.mode = argv[++i];
        } else if (strcmp(argv[i], "--pattern") == 0 && i + 1u < (uint64_t)argc) {
            cfg.pattern = argv[++i];
        } else if (strcmp(argv[i], "--sync") == 0 && i + 1u < (uint64_t)argc) {
            cfg.sync_mode = argv[++i];
        } else if (strcmp(argv[i], "--cache") == 0 && i + 1u < (uint64_t)argc) {
            cfg.cache_policy = argv[++i];
        } else if (strcmp(argv[i], "--block-size") == 0 && i + 1u < (uint64_t)argc) {
            if (parse_u64(argv[++i], &cfg.block_size) != 0) return 2;
        } else if (strcmp(argv[i], "--ops") == 0 && i + 1u < (uint64_t)argc) {
            if (parse_u64(argv[++i], &cfg.ops) != 0) return 2;
        } else if (strcmp(argv[i], "--file-size") == 0 && i + 1u < (uint64_t)argc) {
            if (parse_u64(argv[++i], &cfg.file_size) != 0) return 2;
        } else if (strcmp(argv[i], "--queue-depth") == 0 && i + 1u < (uint64_t)argc) {
            uint64_t qd = 0u;
            if (parse_u64(argv[++i], &qd) != 0 || qd > UINT32_MAX) return 2;
            cfg.queue_depth = (uint32_t)qd;
        } else {
            usage(argv[0]);
            return 2;
        }
    }

    if (cfg.path == NULL || cfg.block_size == 0u || cfg.ops == 0u ||
        cfg.file_size < cfg.block_size || (cfg.file_size % cfg.block_size) != 0u) {
        usage(argv[0]);
        return 2;
    }
    if (cfg.queue_depth != 1u) {
        fprintf(stderr, "queue_depth>1=TOKEN_VAZIO_V1\\n");
        return 3;
    }
    if (strcmp(cfg.mode, "read") != 0 && strcmp(cfg.mode, "write") != 0) {
        return 2;
    }
    if (strcmp(cfg.pattern, "seq") != 0 && strcmp(cfg.pattern, "random") != 0) {
        return 2;
    }
    if (strcmp(cfg.sync_mode, "none") != 0 &&
        strcmp(cfg.sync_mode, "end") != 0 &&
        strcmp(cfg.sync_mode, "each") != 0) {
        return 2;
    }
    if (strcmp(cfg.cache_policy, "os-default") != 0 &&
        strcmp(cfg.cache_policy, "warm") != 0) {
        return 2;
    }

    if (posix_memalign((void **)&buf, 4096u, (size_t)cfg.block_size) != 0) {
        fprintf(stderr, "buffer_allocation=FAIL\\n");
        return 1;
    }

    fd = open(cfg.path, O_RDWR | O_CREAT, 0600);
    if (fd < 0) {
        perror("open");
        goto done;
    }

    if (prepare_file(fd, buf, &cfg) != 0) {
        perror("prepare");
        goto done;
    }

    if (strcmp(cfg.cache_policy, "warm") == 0 && warm_file(fd, buf, &cfg) != 0) {
        perror("warm");
        goto done;
    }

    slots = cfg.file_size / cfg.block_size;
    memset(buf, 0x5A, (size_t)cfg.block_size);

    start_ns = now_ns();
    if (start_ns == 0u) {
        fprintf(stderr, "clock=FAIL\\n");
        goto done;
    }

    for (i = 0u; i < cfg.ops; ++i) {
        uint64_t slot = (strcmp(cfg.pattern, "seq") == 0)
            ? (i % slots)
            : (lcg_next(&rng) % slots);
        off_t off = (off_t)(slot * cfg.block_size);
        int op_rc;

        if (strcmp(cfg.mode, "read") == 0) {
            op_rc = read_full_at(fd, buf, (size_t)cfg.block_size, off);
        } else {
            op_rc = write_full_at(fd, buf, (size_t)cfg.block_size, off);
        }

        if (op_rc != 0) {
            perror("io");
            goto done;
        }

        ++completed;

        if (strcmp(cfg.sync_mode, "each") == 0 && strcmp(cfg.mode, "write") == 0) {
            if (fdatasync(fd) != 0) {
                perror("fdatasync_each");
                goto done;
            }
        }
    }

    if (strcmp(cfg.sync_mode, "end") == 0 && strcmp(cfg.mode, "write") == 0) {
        if (fdatasync(fd) != 0) {
            perror("fdatasync_end");
            goto done;
        }
    }

    end_ns = now_ns();
    if (end_ns <= start_ns) {
        fprintf(stderr, "elapsed=FAIL\\n");
        goto done;
    }

    elapsed_ns = end_ns - start_ns;
    {
        double seconds = (double)elapsed_ns / 1000000000.0;
        double iops = (double)completed / seconds;
        double mib_s = ((double)completed * (double)cfg.block_size) /
                       (seconds * 1024.0 * 1024.0);

        printf(
            "{"
            "\"schema\":\"RMR-IOPS-BENCH-V1\","
            "\"claim_allowed\":false,"
            "\"arch\":\"%s\","
            "\"mode\":\"%s\","
            "\"pattern\":\"%s\","
            "\"sync\":\"%s\","
            "\"cache_policy\":\"%s\","
            "\"direct_io\":false,"
            "\"queue_depth\":%" PRIu32 ","
            "\"block_size\":%" PRIu64 ","
            "\"file_size\":%" PRIu64 ","
            "\"ops_requested\":%" PRIu64 ","
            "\"ops_completed\":%" PRIu64 ","
            "\"elapsed_ns\":%" PRIu64 ","
            "\"iops\":%.3f,"
            "\"mib_per_s\":%.3f,"
            "\"boundary\":\"CI_or_host_measurement_not_cross_device_claim\""
            "}\\n",
            arch_name(),
            cfg.mode,
            cfg.pattern,
            cfg.sync_mode,
            cfg.cache_policy,
            cfg.queue_depth,
            cfg.block_size,
            cfg.file_size,
            cfg.ops,
            completed,
            elapsed_ns,
            iops,
            mib_s);
    }

    rc = 0;

done:
    if (fd >= 0) {
        close(fd);
    }
    free(buf);
    return rc;
}
