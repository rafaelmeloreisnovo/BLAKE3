/*
 * Copyright (c) 2026 Rafael Melo Reis; see rmr/LICENSE_RMR.
 * Hosted Android benchmark orchestration ONLY. BLAKE3 providers and upstream
 * cryptographic source/licensing are unchanged. Not a bare-metal kernel.
 */
package io.rafaelia.rmrhash;

import java.util.Arrays;
import java.util.Locale;

final class BenchmarkCampaignV3 {
    interface Hasher { byte[] digest(int provider, byte[] data); }
    interface Clock { long now(); }
    interface Environment { String snapshot(int bytes, String stage); }

    static final int[] SIZE_ORDER = {4096, 1048576, 16384, 262144, 65536};
    static final int PROVIDERS = 3;
    static final int BLOCKS = 3;
    static final int WARMUPS_PER_BLOCK = 4;
    static final int SAMPLES_PER_BLOCK = 9;
    static final int SAMPLE_COUNT = BLOCKS * SAMPLES_PER_BLOCK;
    private static final String[] NAME = {"UPSTREAM_OFFICIAL", "FORK_ROOT_C", "RMR_PORTABLE_ADAPTER"};
    private static final char[] HEX = "0123456789abcdef".toCharArray();

    private BenchmarkCampaignV3() {}

    private static void require(boolean condition, String reason) {
        if (!condition) throw new IllegalStateException(reason);
    }

    static long median(long[] data) {
        require(data != null && data.length != 0, "EMPTY_SERIES");
        long[] sorted = data.clone();
        Arrays.sort(sorted);
        return sorted[(sorted.length - 1) / 2];
    }

    private static long p95(long[] data) {
        long[] sorted = data.clone();
        Arrays.sort(sorted);
        return sorted[(int) Math.ceil(sorted.length * 0.95) - 1];
    }

    private static long mad(long[] data) {
        long mid = median(data);
        long[] deviations = new long[data.length];
        for (int i = 0; i < data.length; i++) {
            require(data[i] > 0 && mid > 0, "INVALID_DURATION");
            deviations[i] = Math.abs(data[i] - mid);
        }
        return median(deviations);
    }

    private static String hex(byte[] digest) {
        require(digest != null && digest.length == 32, "INVALID_DIGEST_LENGTH");
        char[] result = new char[64];
        for (int i = 0; i < 32; i++) {
            int n = digest[i] & 255;
            result[2 * i] = HEX[n >>> 4];
            result[2 * i + 1] = HEX[n & 15];
        }
        return new String(result);
    }

    private static void appendSeries(StringBuilder out, String key, long[] times) {
        out.append(key).append('=');
        for (int i = 0; i < times.length; i++) {
            if (i != 0) out.append(',');
            out.append(times[i]);
        }
        out.append('\n');
    }

    static String execute(Hasher hasher, Clock clock, Environment environment) {
        require(hasher != null && clock != null && environment != null, "MISSING_DEPENDENCY");
        StringBuilder out = new StringBuilder(20000);
        out.append("RMR_BLAKE3_INDUSTRIAL_BENCH_V3\n")
            .append("SIZE_ORDER_BYTES=4096,1048576,16384,262144,65536\n")
            .append("BLOCKS=3\nWARMUPS_PER_BLOCK=4\nSAMPLES_PER_BLOCK=9\n")
            .append("MEASURED_SAMPLES_PER_PROVIDER_PER_SIZE=27\n")
            .append("ORDER=ROTATING_LATIN_3_BLOCKS_DETERMINISTIC\n")
            .append("ORDER_RULE=PROVIDER_INDEX_(BLOCK*13+ROUND+SLOT)%3\n")
            .append("DATA_RULE=BYTE_I_(I*29+7)_MOD_256\n")
            .append("TIME_SOURCE=SYSTEM_NANOTIME_JNI_INCLUDED\n")
            .append("SAMPLE_EXCLUSION=NONE\n")
            .append("PROTOCOL=HOSTED_ANDROID_THREE_PROVIDERS_SAME_PAYLOAD\n");
        for (int size : SIZE_ORDER) {
            byte[] data = new byte[size];
            for (int i = 0; i < size; i++) data[i] = (byte) (i * 29 + 7);
            byte[] original = data.clone();
            byte[] expected = hasher.digest(0, data);
            hex(expected);
            for (int p = 1; p < PROVIDERS; p++)
                require(Arrays.equals(expected, hasher.digest(p, data)), "PRE_DIGEST_MISMATCH");
            String before = environment.snapshot(size, "BEFORE");
            require(before != null, "ENVIRONMENT_NOT_TYPED");
            out.append("SIZE_BYTES=").append(size).append('\n')
                .append("DATA_DIGEST_").append(size).append('=').append(hex(expected)).append('\n')
                .append("ENV_BEFORE_").append(size).append('=').append(before).append('\n');
            long[][] ns = new long[PROVIDERS][SAMPLE_COUNT];
            int[] counts = new int[PROVIDERS];
            for (int block = 0; block < BLOCKS; block++) {
                for (int round = 0; round < WARMUPS_PER_BLOCK + SAMPLES_PER_BLOCK; round++) {
                    for (int slot = 0; slot < PROVIDERS; slot++) {
                        int p = (block * (WARMUPS_PER_BLOCK + SAMPLES_PER_BLOCK) + round + slot) % PROVIDERS;
                        long start = clock.now();
                        byte[] observed = hasher.digest(p, data);
                        long end = clock.now();
                        require(end > start, "CLOCK_NON_MONOTONIC_OR_ZERO");
                        require(Arrays.equals(expected, observed), "TIMED_DIGEST_MISMATCH");
                        if (round >= WARMUPS_PER_BLOCK) {
                            int index = counts[p]++;
                            require(index < SAMPLE_COUNT, "SAMPLE_OVERFLOW");
                            ns[p][index] = end - start;
                            require(ns[p][index] > 0, "DURATION_OVERFLOW");
                        }
                    }
                }
            }
            require(Arrays.equals(original, data), "PAYLOAD_MUTATED");
            for (int p = 0; p < PROVIDERS; p++) {
                require(counts[p] == SAMPLE_COUNT, "INCOMPLETE_SAMPLES");
                require(Arrays.equals(expected, hasher.digest(p, data)), "POST_DIGEST_MISMATCH");
            }
            String after = environment.snapshot(size, "AFTER");
            require(after != null, "ENVIRONMENT_NOT_TYPED");
            out.append("ENV_AFTER_").append(size).append('=').append(after).append('\n');
            long baseline = median(ns[0]);
            require(baseline > 0, "INVALID_BASELINE");
            for (int p = 0; p < PROVIDERS; p++) {
                String key = size + "_" + NAME[p];
                appendSeries(out, "RAW_NS_" + key, ns[p]);
                out.append("MEDIAN_NS_").append(key).append('=').append(median(ns[p])).append('\n')
                    .append("MAD_NS_").append(key).append('=').append(mad(ns[p])).append('\n')
                    .append("P95_NS_").append(key).append('=').append(p95(ns[p])).append('\n')
                    .append("MEDIAN_RATIO_TO_OFFICIAL_").append(key).append('=')
                    .append(String.format(Locale.US, "%.6f", (double) median(ns[p]) / baseline)).append('\n');
                long[] early = Arrays.copyOfRange(ns[p], 0, SAMPLES_PER_BLOCK);
                long[] late = Arrays.copyOfRange(ns[p], 2 * SAMPLES_PER_BLOCK, SAMPLE_COUNT);
                out.append("FIRST_BLOCK_MEDIAN_NS_").append(key).append('=').append(median(early)).append('\n')
                    .append("LAST_BLOCK_MEDIAN_NS_").append(key).append('=').append(median(late)).append('\n');
            }
        }
        out.append("EQUALITY=PASS_SCOPED_ALL_SIZES\n")
            .append("TOTAL_MEASURED_SAMPLES=405\n")
            .append("THERMAL_OR_CLOCK_CONTROL=NOT_PERFORMED\n")
            .append("CPU_GOVERNOR=TOKEN_VAZIO_UNVERIFIED\n")
            .append("CONFIDENCE_INTERVAL=NOT_COMPUTED\n")
            .append("RESULT_SCOPE=ON_DEVICE_OBSERVATION_NOT_PROVIDER_ATTESTATION\n")
            .append("CLAIM_ALLOWED=false\n");
        return out.toString();
    }
}
