/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * Licensed under rmr/LICENSE_RMR.
 * Android/JNI adapter only; BLAKE3 semantics remain upstream.
 */
package io.rafaelia.rmrhash;

final class NativeHash {
    static { System.loadLibrary("rmrhash"); }
    private NativeHash() {}

    static native boolean selfTest();
    static native byte[] digest(byte[] input);
}
