/*
 * Copyright (c) 2026 Rafael Melo Reis. Licensed under rmr/LICENSE_RMR.
 * BLAKE3 (all providers): BLAKE3-team original authors, upstream licenses.
 * Offline research UI: NO network, secrets, storage or device benchmark claims.
 */
package io.rafaelia.rmrhash;

import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.BatteryManager;
import android.os.PowerManager;
import android.os.SystemClock;
import android.content.IntentFilter;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Locale;

public final class MainActivity extends Activity {
    private static final int MAX_BYTES = 1048576;
    private static final String KAT_ABC =
        "6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85";
    private static final String KAT_EMPTY =
        "af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262";
    private static final String[] NAMES = {"UPSTREAM_OFFICIAL", "FORK_ROOT_C", "RMR_PORTABLE_ADAPTER"};
    private static final char[] HEX = "0123456789abcdef".toCharArray();

    private TextView result;
    private EditText input;
    private Button hashButton, smallButton, bigButton, industrialButton, shareButton;
    private volatile boolean busy = false;
    private boolean verified;
    private String lastReceipt = "STATE=NOT_RUN\nCLAIM_ALLOWED=false";

    private static byte[] digest(int provider, byte[] data) {
        if (provider == 0) return NativeHash.officialDigest(data);
        if (provider == 1) return NativeHash.forkDigest(data);
        return NativeHash.digest(data);
    }

    private static String hex(byte[] bytes) {
        if (bytes == null || bytes.length != 32) return "INVALID_DIGEST";
        char[] out = new char[64];
        for (int k = 0; k < 32; k++) {
            int n = bytes[k] & 255;
            out[k * 2] = HEX[n >>> 4];
            out[k * 2 + 1] = HEX[n & 15];
        }
        return new String(out);
    }

    private static boolean equalsKnown(String expected, byte[] actual) {
        return expected.equals(hex(actual));
    }

    private static boolean runAllKats() {
        byte[] empty = new byte[0];
        byte[] abc = "abc".getBytes(StandardCharsets.UTF_8);
        if (!NativeHash.selfTest()) return false;
        for (int p = 0; p < 3; p++) {
            if (!equalsKnown(KAT_EMPTY, digest(p, empty))) return false;
            if (!equalsKnown(KAT_ABC, digest(p, abc))) return false;
        }
        byte[] check = new byte[1025];
        for (int i = 0; i < check.length; i++) check[i] = (byte)(i * 31 + 11);
        for (int length : new int[]{63, 64, 1023, 1024, 1025}) {
            byte[] data = Arrays.copyOf(check, length);
            byte[] expected = digest(0, data);
            for (int p = 1; p < 3; p++) {
                if (!Arrays.equals(expected, digest(p, data))) return false;
            }
        }
        return true;
    }

    private String environment() {
        String abi = Build.SUPPORTED_ABIS.length == 0 ? "TOKEN_VAZIO" : Build.SUPPORTED_ABIS[0];
        return "ABI=" + abi
            + "\nANDROID_SDK=" + Build.VERSION.SDK_INT
            + "\nMODEL=" + Build.MANUFACTURER + "/" + Build.MODEL
            + "\nFORK_SOURCE_SHA=" + BuildConfig.SOURCE_SHA
            + "\nOFFICIAL_SOURCE_SHA=" + BuildConfig.OFFICIAL_SHA
            + "\nRMR_ADAPTER_PATH=rmr/portable/provider/blake3"
            + "\nLIBRARIES=libofficialhash.so,libforkhash.so,librmrhash.so"
            + "\nBUILD=JNI_ANDROID_HOSTED_NOT_BARE_METAL"
            + "\nCLAIM_ALLOWED=false";
    }

    @Override public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ScrollView scroll = new ScrollView(this);
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        int p = Math.round(16f * getResources().getDisplayMetrics().density);
        root.setPadding(p, p, p, p);
        scroll.addView(root);
        TextView heading = new TextView(this);
        heading.setText("BLAKE3 · Três caminhos / Dois ABIs\nOficial × Fork × RMR");
        heading.setTextSize(21f);
        root.addView(heading);
        TextView disclaimer = new TextView(this);
        disclaimer.setText("Mesmo algoritmo. Três implementações/rotas de compilação; bibliotecas isoladas. " +
            "Comparação válida somente com digests iguais, na ABI efetivamente carregada. " +
            "Licenças e origem disponíveis abaixo. Sem rede ou permissões adicionais.");
        root.addView(disclaimer);
        input = new EditText(this);
        input.setText("abc");
        input.setMinLines(2);
        input.setHint("Texto UTF-8 para hash em cada biblioteca");
        root.addView(input);
        hashButton = addButton(root, "Comparar hashes do texto", v -> compareText());
        smallButton = addButton(root, "Benchmark local: 4 KiB", v -> benchmark(4096));
        bigButton = addButton(root, "Benchmark local: 1 MiB", v -> benchmark(1048576));
        industrialButton = addButton(root, "Campanha industrial V3: 5 tamanhos + amostras", v -> industrialCampaign());
        shareButton = addButton(root, "Compartilhar recibo em texto", v -> shareReceipt());
        addButton(root, "Ver atribuições e licenças completas", v -> showNotices());
        result = new TextView(this);
        result.setTextIsSelectable(true);
        root.addView(result);
        setContentView(scroll);
        try {
            verified = runAllKats();
            String msg = "KAT_ABC_EMPTY= " + (verified ? "PASS" : "FAIL")
                + "\nCROSS_BACKEND_BOUNDARIES= " + (verified ? "PASS" : "FAIL")
                + "\n" + environment();
            result.setText(msg);
            lastReceipt = msg;
        } catch (LinkageError | RuntimeException e) {
            verified = false;
            result.setText("FAIL_CLOSED: native loading or KAT failed: " + e.getClass().getSimpleName()
                + "\n" + environment());
        }
        setEnabledForHash(verified);
    }

    private Button addButton(LinearLayout parent, String title, View.OnClickListener action) {
        Button b = new Button(this);
        b.setText(title);
        parent.addView(b);
        b.setOnClickListener(action);
        return b;
    }

    private void setEnabledForHash(boolean on) {
        hashButton.setEnabled(on);
        smallButton.setEnabled(on);
        bigButton.setEnabled(on);
        industrialButton.setEnabled(on);
        shareButton.setEnabled(on && !busy);
    }

    private void compareText() {
        if (!verified || busy) return;
        byte[] data = input.getText().toString().getBytes(StandardCharsets.UTF_8);
        if (data.length > MAX_BYTES) {
            result.setText("BLOCKED: payload exceeds 1 MiB UTF-8"); return;
        }
        try {
            StringBuilder b = new StringBuilder("TRIPLE_DIGEST_COMPARE\n");
            byte[] first = null;
            boolean equal = true;
            for (int p = 0; p < 3; p++) {
                byte[] d = digest(p, data);
                if (d == null || d.length != 32) { equal = false; break; }
                if (first == null) first = d;
                else if (!Arrays.equals(first, d)) equal = false;
                b.append(NAMES[p]).append("=").append(hex(d)).append("\n");
            }
            b.append("RESULT=").append(equal ? "ALL_DIGESTS_EQUAL" : "FAIL_CLOSED_MISMATCH").append("\n");
            b.append("INPUT_BYTES=").append(data.length).append("\n").append(environment());
            lastReceipt = b.toString();
            result.setText(lastReceipt);
        } catch (RuntimeException | LinkageError e) {
            result.setText("BLOCKED: " + e.getClass().getSimpleName());
        }
    }

    private static long median(long[] times) {
        long[] sorted = times.clone();
        Arrays.sort(sorted);
        return sorted[sorted.length / 2];
    }

    private void benchmark(int n) {
        if (!verified || busy) return;
        busy = true;
        setEnabledForHash(false);
        result.setText("Benchmarking on this device (same ABI): " + n + " bytes");
        new Thread(() -> {
            String output;
            try {
                byte[] data = new byte[n];
                for (int i = 0; i < n; i++) data[i] = (byte)(i * 29 + 7);
                byte[][] reference = new byte[3][];
                for (int p = 0; p < 3; p++) {
                    reference[p] = digest(p, data);
                    if (reference[p] == null || reference[p].length != 32
                        || !Arrays.equals(reference[0], reference[p])) {
                        throw new IllegalStateException("PRE_BENCH_DIGEST_MISMATCH");
                    }
                }
                final int warmups = 4, rounds = 9;
                long[][] durations = new long[3][rounds];
                // Rotate measurement order so one backend is not always first.
                for (int j = 0; j < warmups + rounds; j++) {
                    for (int slot = 0; slot < 3; slot++) {
                        int idx = (j + slot) % 3;
                        long start = System.nanoTime();
                        byte[] resultData = digest(idx, data);
                        long elapsed = System.nanoTime() - start;
                        if (!Arrays.equals(reference[idx], resultData))
                            throw new IllegalStateException("POST_BENCH_DIGEST_MISMATCH");
                        if (j >= warmups) durations[idx][j - warmups] = elapsed;
                    }
                }
                long baseline = median(durations[0]);
                StringBuilder b = new StringBuilder("RMR_BLAKE3_TRIPLE_BENCH_V1\n");
                b.append("BYTE_COUNT=").append(n).append("\n")
                    .append("MEDIAN_SAMPLES=").append(rounds).append("\n")
                    .append("WARMUPS=").append(warmups).append("\n")
                    .append("DATA_DIGEST=").append(hex(reference[0])).append("\n");
                for (int p = 0; p < 3; p++) {
                    long med = median(durations[p]);
                    double ratio = baseline == 0 ? Double.NaN : ((double)med / baseline);
                    b.append(NAMES[p]).append("_MEDIAN_NS=").append(med).append("\n");
                    b.append(NAMES[p]).append("_VS_OFFICIAL_TIME_RATIO=")
                        .append(String.format(Locale.US, "%.4f", ratio)).append("\n");
                }
                b.append("EQUALITY=PASS\nTIMING=ANDROID_NANOTIME_JNI_INCLUDED\n")
                    .append("THERMAL_OR_CLOCK_CONTROL=NOT_PERFORMED\n")
                    .append("PHYSICAL_RUN=ON_DEVICE_OBSERVATION_ONLY_NOT_PROVIDER_ATTESTATION\n")
                    .append(environment());
                output = b.toString();
            } catch (RuntimeException | LinkageError e) {
                output = "BLOCKED_BENCH=" + e.getClass().getSimpleName()
                    + "\nCLAIM_ALLOWED=false\n" + environment();
            }
            final String receipt = output;
            runOnUiThread(() -> {
                busy = false;
                lastReceipt = receipt;
                result.setText(receipt);
                setEnabledForHash(verified);
            });
        }, "b3-compare").start();
    }

    private String industrialSnapshot(int size, String phase) {
        PowerManager pm = (PowerManager) getSystemService(POWER_SERVICE);
        String thermal = pm == null ? "TOKEN_VAZIO_UNAVAILABLE"
            : Integer.toString(pm.getCurrentThermalStatus());
        Intent state = registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        int batteryTemp = state == null ? Integer.MIN_VALUE
            : state.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, Integer.MIN_VALUE);
        String temperature = batteryTemp == Integer.MIN_VALUE ? "TOKEN_VAZIO_UNAVAILABLE"
            : Integer.toString(batteryTemp);
        return "phase:" + phase + ";bytes:" + size
            + ";elapsed_realtime_ns:" + SystemClock.elapsedRealtimeNanos()
            + ";wall_clock_ms_untrusted:" + System.currentTimeMillis()
            + ";thermal_status_android:" + thermal
            + ";battery_temp_tenths_c_not_cpu:" + temperature
            + ";cpu_frequency: TOKEN_VAZIO_NOT_READ";
    }

    private void industrialCampaign() {
        if (!verified || busy) return;
        busy = true;
        setEnabledForHash(false);
        result.setText("Campanha V3: igualdade + cinco tamanhos + 27 amostras/provedor/tamanho");
        new Thread(() -> {
            String receipt;
            try {
                String resultText = BenchmarkCampaignV3.execute(
                    MainActivity::digest, System::nanoTime, this::industrialSnapshot);
                boolean is64 = android.os.Process.is64Bit();
                String[] candidateAbis = is64 ? Build.SUPPORTED_64_BIT_ABIS : Build.SUPPORTED_32_BIT_ABIS;
                String actualAbi = candidateAbis.length > 0 ? candidateAbis[0] : "TOKEN_VAZIO_ABI";
                receipt = resultText
                    + "EFFECTIVE_PROCESS_BITS=" + (is64 ? 64 : 32) + "\n"
                    + "EFFECTIVE_PROCESS_ABI_CANDIDATE=" + actualAbi + "\n"
                    + "APK_SHA256=TOKEN_VAZIO_MATCH_WITH_CI_ARTIFACT\n"
                    + "ENVIRONMENT_SNAPSHOTS=OBSERVED_NOT_CONTROLLED\n"
                    + environment();
            } catch (RuntimeException | LinkageError e) {
                receipt = "RMR_BLAKE3_INDUSTRIAL_BENCH_V3\n"
                    + "STATUS=BLOCKED_FAIL_CLOSED\n"
                    + "CAUSE=" + e.getClass().getSimpleName() + "\n"
                    + "RAW_SAMPLES=DISCARDED_PARTIAL\n"
                    + "CLAIM_ALLOWED=false\n"
                    + environment();
            }
            String resultReceipt = receipt;
            runOnUiThread(() -> {
                busy = false;
                lastReceipt = resultReceipt;
                result.setText(resultReceipt);
                setEnabledForHash(verified);
            });
        }, "rmr-bench-v3").start();
    }

    private void shareReceipt() {
        if (lastReceipt == null || !verified || busy) return;
        Intent send = new Intent(Intent.ACTION_SEND);
        send.setType("text/plain");
        send.putExtra(Intent.EXTRA_TEXT, lastReceipt);
        startActivity(Intent.createChooser(send, "Exportar recibo de benchmark"));
    }

    private void showNotices() {
        final String[] files = {
            "PROVENANCE.txt", "OFFICIAL_LICENSE_A2", "OFFICIAL_LICENSE_A2LLVM",
            "OFFICIAL_LICENSE_CC0", "LICENSE_A2", "LICENSE_A2LLVM", "LICENSE_CC0",
            "LICENSE_RMR", "LICENSE_RMR_INDIVIDUAL_RESEARCH_V1.txt",
            "THIRD_PARTY_NOTICES_RAFAELIA.md"
        };
        final StringBuilder licenses = new StringBuilder("NOTICES / FULL TEXT\n");
        try {
            for (String file : files) {
                licenses.append("\n======== ").append(file).append(" ========\n");
                try (InputStream in = getAssets().open(file)) {
                    ByteArrayOutputStream b = new ByteArrayOutputStream();
                    byte[] buffer = new byte[4096];
                    int got;
                    while ((got = in.read(buffer)) != -1) b.write(buffer, 0, got);
                    licenses.append(b.toString("UTF-8"));
                }
            }
            new android.app.AlertDialog.Builder(this).setTitle("Licenças e autoria")
                .setMessage(licenses.toString()).setPositiveButton("Fechar", null).show();
        } catch (Exception e) {
            result.setText("LICENSE_ASSETS_MISSING_FAIL_CLOSED: " + e.getClass().getSimpleName());
        }
    }
}
