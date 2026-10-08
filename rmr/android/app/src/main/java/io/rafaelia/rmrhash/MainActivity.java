/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * Licensed under rmr/LICENSE_RMR.
 */
package io.rafaelia.rmrhash;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import java.nio.charset.StandardCharsets;

public final class MainActivity extends Activity {
    private static final int MAX_INPUT_BYTES = 1048576;
    private final char[] hex = "0123456789abcdef".toCharArray();
    private EditText input;
    private TextView report;
    private boolean katOk;

    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        ScrollView scroller = new ScrollView(this);
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        int pad = (int)(18 * getResources().getDisplayMetrics().density);
        layout.setPadding(pad, pad, pad, pad);
        scroller.addView(layout);

        TextView heading = new TextView(this);
        heading.setText("RMR BLAKE3 Laboratory\nAndroid hosted / C provider / freestanding core");
        heading.setTextSize(20f);
        layout.addView(heading);

        input = new EditText(this);
        input.setHint("UTF-8 text to hash");
        input.setText("abc");
        input.setMinLines(3);
        layout.addView(input);

        Button hash = new Button(this);
        hash.setText("BLAKE3-256 / verify");
        layout.addView(hash);

        report = new TextView(this);
        report.setTextIsSelectable(true);
        layout.addView(report);
        setContentView(scroller);

        try {
            katOk = NativeHash.selfTest();
            report.setText("KAT(abc,empty)=" + (katOk ? "PASS" : "FAIL") +
                "\nSOURCE_SHA=" + BuildConfig.SOURCE_SHA +
                "\nABI=" + android.os.Build.SUPPORTED_ABIS[0] +
                "\nAPPLIANCE=ANDROID_PLATFORM_LINKED" +
                "\nPHYSICAL_DEVICE_RECEIPT=TOKEN_VAZIO");
        } catch (LinkageError e) {
            katOk = false;
            report.setText("NATIVE_LOAD=FAIL\n" + e.getClass().getSimpleName());
        }
        hash.setEnabled(katOk);
        hash.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) { hashInput(); }
        });
    }

    private void hashInput() {
        if (!katOk) { report.setText("FAIL_CLOSED: KAT failed"); return; }
        byte[] bytes = input.getText().toString().getBytes(StandardCharsets.UTF_8);
        if (bytes.length > MAX_INPUT_BYTES) {
            report.setText("REJECTED: input exceeds 1048576 UTF-8 bytes");
            return;
        }
        try {
            byte[] digest = NativeHash.digest(bytes);
            if (digest == null || digest.length != 32) {
                report.setText("DIGEST=FAIL\nCLAIM_ALLOWED=false");
                return;
            }
            char[] out = new char[64];
            for (int i = 0; i < digest.length; i++) {
                out[2 * i] = hex[(digest[i] >>> 4) & 15];
                out[2 * i + 1] = hex[digest[i] & 15];
            }
            report.setText("BLAKE3-256=" + new String(out) +
                "\nINPUT_UTF8_BYTES=" + bytes.length +
                "\nKAT(abc,empty)=PASS" +
                "\nSOURCE_SHA=" + BuildConfig.SOURCE_SHA +
                "\nABI=" + android.os.Build.SUPPORTED_ABIS[0] +
                "\nDEVICE_RECEIPT=TOKEN_VAZIO");
        } catch (LinkageError e) {
            report.setText("JNI_RUNTIME=FAIL\n" + e.getClass().getSimpleName());
        }
    }
}
