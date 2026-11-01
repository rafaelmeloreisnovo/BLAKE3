/*
 * RMR ZIPRAF FREE Android client (c) 2026 Rafael Melo Reis.
 * Licensed under rmr/LICENSE_RMR. BLAKE3 source retains upstream licenses.
 * Storage Access Framework is Android-hosted; pure C codec has no OS I/O.
 */
package io.rafaelia.rmrfree;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public final class MainActivity extends Activity {
    private static final int READ = 1001;
    private static final int WRITE = 1002;
    private static final int MAX_FILE = 3 * 1024 * 1024;
    private static final String ABC =
        "6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85";
    private static final String EMPTY =
        "af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262";
    private static native byte[] nativeDigest(byte[] input);
    private static native byte[] nativePack(byte[] input);
    private static native byte[] nativeUnpack(byte[] input);

    private TextView status;
    private Button packButton, unpackButton, saveButton;
    private volatile byte[] fileBytes;
    private volatile byte[] outputBytes;
    private volatile boolean busy;
    private volatile boolean verified;
    private String saveName = "output.zrf1";

    private static String hex(byte[] value) {
        if (value == null || value.length != 32) return "TOKEN_VAZIO_DIGEST";
        final char[] chars = "0123456789abcdef".toCharArray();
        char[] out = new char[64];
        for (int i=0; i<32; ++i) {
            int v = value[i] & 255;
            out[2*i] = chars[v>>>4];
            out[2*i+1] = chars[v&15];
        }
        return new String(out);
    }
    private static boolean kat() {
        byte[] abc = "abc".getBytes(StandardCharsets.UTF_8);
        byte[] blank = new byte[0];
        if (!ABC.equals(hex(nativeDigest(abc))) ||
            !EMPTY.equals(hex(nativeDigest(blank)))) return false;
        byte[] packed = nativePack(abc);
        return packed != null && Arrays.equals(abc,nativeUnpack(packed));
    }
    private Button button(LinearLayout root, String text, View.OnClickListener click) {
        Button b = new Button(this);
        b.setText(text); b.setOnClickListener(click); root.addView(b);
        return b;
    }
    private void info(String line) { status.setText(line); }
    private void controls() {
        boolean loaded = verified && !busy && fileBytes != null;
        packButton.setEnabled(loaded && fileBytes.length <= MAX_FILE);
        unpackButton.setEnabled(loaded && fileBytes.length >= 46);
        saveButton.setEnabled(verified && !busy && outputBytes != null);
    }
    @Override public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        ScrollView scroll = new ScrollView(this);
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        int pad=(int)(16*getResources().getDisplayMetrics().density);
        root.setPadding(pad,pad,pad,pad); scroll.addView(root);
        TextView title = new TextView(this);
        title.setText("RMR ZIPRAF FREE · BLAKE3\n1 aplicativo · ARM32/ARM64");
        title.setTextSize(21f); root.addView(title);
        TextView notice = new TextView(this);
        notice.setText("Cliente local sem rede e sem permissões de armazenamento. " +
            "ZRF1 é experimental e NÃO comprovadamente compatível com ZIPRAF privado. " +
            "BLAKE3 verifica integridade, não assinatura/autenticidade.");
        root.addView(notice);
        button(root,"Selecionar arquivo (até 3 MiB)", v -> pick());
        packButton=button(root,"Compactar em ZRF1", v -> process(true));
        unpackButton=button(root,"Extrair ZRF1 e verificar BLAKE3", v -> process(false));
        saveButton=button(root,"Salvar resultado no Android", v -> save());
        status=new TextView(this); status.setTextIsSelectable(true); root.addView(status);
        setContentView(scroll);
        try {
            System.loadLibrary("rmrfree");
            verified=kat();
            info("SELFTEST="+(verified?"PASS":"FAIL_CLOSED")+
                 "\nABI="+Arrays.toString(Build.SUPPORTED_ABIS)+
                 "\nMAX_INPUT_BYTES="+MAX_FILE+
                 "\nFORMAT=ZRF1/EXPERIMENTAL"+
                 "\nSIGNED_RELEASE=TOKEN_VAZIO"+
                 "\nCLAIM_ALLOWED=false");
        } catch (LinkageError | RuntimeException failure) {
            verified=false;
            info("FAIL_CLOSED_NATIVE="+failure.getClass().getSimpleName());
        }
        controls();
    }
    private void pick() {
        if (!verified || busy) return;
        Intent i = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        i.addCategory(Intent.CATEGORY_OPENABLE);
        i.setType("*/*");
        i.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        startActivityForResult(i,READ);
    }
    private void save() {
        if (outputBytes == null || busy) return;
        Intent i = new Intent(Intent.ACTION_CREATE_DOCUMENT);
        i.addCategory(Intent.CATEGORY_OPENABLE);
        i.setType("application/octet-stream");
        i.putExtra(Intent.EXTRA_TITLE,saveName);
        startActivityForResult(i,WRITE);
    }
    private static byte[] load(InputStream in) throws java.io.IOException {
        ByteArrayOutputStream out=new ByteArrayOutputStream();
        byte[] tmp=new byte[8192];
        int n;
        while ((n=in.read(tmp))!=-1) {
            if (out.size() + n > MAX_FILE+46)
                throw new java.io.IOException("FILE_OVER_3MIB_CAP");
            out.write(tmp,0,n);
        }
        return out.toByteArray();
    }
    @Override protected void onActivityResult(int request, int result, Intent intent) {
        super.onActivityResult(request,result,intent);
        if (result!=RESULT_OK || intent==null || intent.getData()==null) return;
        final Uri uri=intent.getData();
        if (request==READ) {
            busy=true; controls(); info("Lendo arquivo local...");
            new Thread(() -> {
                byte[] bytes=null; String state;
                try (InputStream in=getContentResolver().openInputStream(uri)) {
                    if (in==null) throw new java.io.IOException("NO_INPUT");
                    bytes=load(in);
                    state="FILE_READ=PASS\nBYTES="+bytes.length;
                } catch (Exception ex) {
                    state="FILE_READ=BLOCKED_"+ex.getClass().getSimpleName();
                }
                final byte[] newData=bytes; final String message=state;
                runOnUiThread(() -> {
                    fileBytes=newData;outputBytes=null;busy=false;
                    info(message);controls();
                });
            },"rmr-free-read").start();
        } else if (request==WRITE) {
            final byte[] bytes=outputBytes;
            if (bytes==null) return;
            busy=true;controls();
            new Thread(() -> {
                String state;
                try (OutputStream out=getContentResolver().openOutputStream(uri)) {
                    if (out==null) throw new java.io.IOException("NO_OUTPUT");
                    out.write(bytes);out.flush();
                    state="FILE_SAVED=PASS\nBYTES="+bytes.length;
                } catch (Exception ex) {
                    state="FILE_SAVED=BLOCKED_"+ex.getClass().getSimpleName();
                }
                final String message=state;
                runOnUiThread(() -> {
                    busy=false;info(message+"\nDIGEST_OF_OUTPUT="+hex(nativeDigest(bytes)));
                    controls();
                });
            },"rmr-free-write").start();
        }
    }
    private void process(boolean packing) {
        final byte[] input=fileBytes;
        if (!verified || busy || input==null) return;
        busy=true;controls();info(packing?"Compactando...":"Extraindo...");
        new Thread(() -> {
            String state;byte[] result=null;
            try {
                result=packing ? nativePack(input) : nativeUnpack(input);
                if (result==null) throw new IllegalStateException("CODEC_REJECTED");
                if (packing && !Arrays.equals(input,nativeUnpack(result)))
                    throw new IllegalStateException("ROUNDTRIP_MISMATCH");
                state=(packing?"PACK":"UNPACK")+"=PASS\n"+
                    "SOURCE_BYTES="+input.length+"\nOUTPUT_BYTES="+result.length+
                    "\nBLAKE3_OUTPUT="+hex(nativeDigest(result))+
                    "\nINTEGRITY=PASS_SCOPED\nAUTHENTICITY=NOT_PROVEN"+
                    "\nDEVICE_ATTESTATION=NOT_PERFORMED";
            } catch (RuntimeException | LinkageError failure) {
                state="FAIL_CLOSED="+failure.getClass().getSimpleName()+
                    "\nCLAIM_ALLOWED=false";
                result=null;
            }
            final byte[] bytes=result; final String message=state;
            runOnUiThread(() -> {
                outputBytes=bytes;saveName=packing?"packed.zrf1":"restored.bin";
                busy=false;info(message);controls();
            });
        },"rmr-free-codec").start();
    }
}
