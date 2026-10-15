/*
 * Android/JNI comparison adapter (c) 2026 Rafael Melo Reis.
 * RMR LICENSE_RMR applies to this wrapper; BLAKE3 algorithm retains original
 * upstream copyright and licensing. This is NOT a freestanding library.
 */
#include <jni.h>
#include <stddef.h>
#include <stdint.h>
#include "blake3.h"

#define MAX_BYTES 1048576

static jbyteArray digest_bytes(JNIEnv *env, jbyteArray input) {
    jbyte *locked = NULL;
    jbyteArray result = NULL;
    uint8_t digest[BLAKE3_OUT_LEN];
    blake3_hasher hasher;
    jsize length;
    if (input == NULL) return NULL;
    length = (*env)->GetArrayLength(env, input);
    if (length < 0 || length > MAX_BYTES) return NULL;
    if (length != 0) {
        locked = (*env)->GetByteArrayElements(env, input, NULL);
        if (locked == NULL) return NULL;
    }
    blake3_hasher_init(&hasher);
    /* For an empty buffer, the one-shot update can be skipped. */
    if (length > 0) blake3_hasher_update(&hasher, locked, (size_t)length);
    blake3_hasher_finalize(&hasher, digest, BLAKE3_OUT_LEN);
    if (locked != NULL) (*env)->ReleaseByteArrayElements(env, input, locked, JNI_ABORT);
    result = (*env)->NewByteArray(env, BLAKE3_OUT_LEN);
    if (result == NULL) return NULL;
    (*env)->SetByteArrayRegion(env, result, 0, BLAKE3_OUT_LEN, (const jbyte *)digest);
    if ((*env)->ExceptionCheck(env)) return NULL;
    return result;
}

/* One wrapper compiled separately for each independently sourced library. */
#if defined(RMR_COMPARE_OFFICIAL)
JNIEXPORT jbyteArray JNICALL
Java_io_rafaelia_rmrhash_NativeHash_officialDigest(JNIEnv *env, jclass owner, jbyteArray input) {
    (void)owner;
    return digest_bytes(env, input);
}
#elif defined(RMR_COMPARE_FORK)
JNIEXPORT jbyteArray JNICALL
Java_io_rafaelia_rmrhash_NativeHash_forkDigest(JNIEnv *env, jclass owner, jbyteArray input) {
    (void)owner;
    return digest_bytes(env, input);
}
#else
#error "Specify exactly one comparison provider"
#endif
