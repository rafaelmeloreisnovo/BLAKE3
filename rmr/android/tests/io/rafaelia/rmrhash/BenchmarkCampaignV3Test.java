/* Copyright (c) 2026 Rafael Melo Reis; rmr/LICENSE_RMR.
 * Deterministic hosted harness tests. Synthetic durations are NOT phone benchmarks. */
package io.rafaelia.rmrhash;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;

public final class BenchmarkCampaignV3Test {
    static final class FakeClock implements BenchmarkCampaignV3.Clock {
        long nanos = 1000;
        @Override public long now() { return nanos; }
        void add(int p) { nanos += 100 + 50 * p; }
    }
    static byte[] sha(byte[] data) {
        try { return MessageDigest.getInstance("SHA-256").digest(data); }
        catch (NoSuchAlgorithmException e) { throw new AssertionError(e); }
    }
    static void check(boolean good, String why) { if (!good) throw new AssertionError(why); }
    static void mustFail(Runnable run, String label) {
        boolean blocked = false;
        try { run.run(); } catch (IllegalStateException e) { blocked = true; }
        check(blocked, label);
    }
    public static void main(String[] args) {
        FakeClock clock = new FakeClock();
        String result = BenchmarkCampaignV3.execute((p, data) -> {
            clock.add(p);
            return sha(data);
        }, clock, (size, stage) -> "SYNTHETIC_TEST_ENV_" + stage);
        check(result.contains("TOTAL_MEASURED_SAMPLES=405"), "sample total");
        check(result.contains("EQUALITY=PASS_SCOPED_ALL_SIZES"), "equality gate");
        check(result.contains("THERMAL_OR_CLOCK_CONTROL=NOT_PERFORMED"), "claim gate");
        check(result.contains("CLAIM_ALLOWED=false"), "claim denied");
        for (int size : BenchmarkCampaignV3.SIZE_ORDER) {
            for (String provider : new String[]{"UPSTREAM_OFFICIAL", "FORK_ROOT_C", "RMR_PORTABLE_ADAPTER"}) {
                String k = "RAW_NS_" + size + "_" + provider + "=";
                int start = result.indexOf(k);
                check(start >= 0, "missing raw series: " + k);
                String[] samples = result.substring(start + k.length(), result.indexOf('\n', start)).split(",");
                check(samples.length == 27, "raw samples must be 27");
                for (String ns : samples) check(Long.parseLong(ns) > 0, "nonpositive duration");
            }
        }
        check(BenchmarkCampaignV3.median(new long[]{3, 99, 4}) == 4, "median");
        mustFail(() -> BenchmarkCampaignV3.median(new long[0]), "empty median");
        FakeClock bad = new FakeClock();
        mustFail(() -> BenchmarkCampaignV3.execute((p, data) -> sha(data), bad, (s,t) -> "NA"), "stopped clock");
        FakeClock mismatchClock = new FakeClock();
        mustFail(() -> BenchmarkCampaignV3.execute((p, data) -> {
            mismatchClock.add(p);
            byte[] resultDigest = sha(data);
            if (p == 1) resultDigest[0] ^= 1;
            return resultDigest;
        }, mismatchClock, (s,t) -> "NA"), "provider mismatch");
        FakeClock mutationClock = new FakeClock();
        mustFail(() -> BenchmarkCampaignV3.execute((p, data) -> {
            mutationClock.add(p);
            if (p == 2) data[0] ^= 1;
            return sha(data);
        }, mutationClock, (s,t) -> "NA"), "payload mutation");
        mustFail(() -> BenchmarkCampaignV3.execute(null, bad, (s,t) -> "NA"), "missing adapter");
        System.out.println("RMR_BENCH_V3_HOST_FAKE_CLOCK_TESTS=PASS_SCOPED");
        System.out.println("DEVICE_EXECUTION=NOT_RUN; CRYPTO_PROVIDER_TEST=NOT_RUN; CLAIM_ALLOWED=false");
    }
}
