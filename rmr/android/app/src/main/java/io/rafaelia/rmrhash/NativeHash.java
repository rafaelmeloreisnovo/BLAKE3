/*
 * Android JNI harness, copyright (c) 2026 Rafael Melo Reis.
 * rmr/LICENSE_RMR applies to the adapter. All BLAKE3 cores remain upstream.
 * Three binary provider paths; not three cryptographic algorithms.
 */
package io.rafaelia.rmrhash;

final class NativeHash {
    static {
        System.loadLibrary("officialhash");
        System.loadLibrary("forkhash");
        System.loadLibrary("rmrhash");
    }
    private NativeHash() {}

    static native byte[] officialDigest(byte[] input);
    static native byte[] forkDigest(byte[] input);
    static native boolean selfTest(); // Existing RMR adapter KAT; unchanged JNI binding.
    static native byte[] digest(byte[] input); // Existing RMR portable adapter.
}
