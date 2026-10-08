/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * Licensed under rmr/LICENSE_RMR.
 * This file is the hosted Android boundary, NOT a freestanding ELF.
 */
#include <jni.h>
#include "rmr_portable_blake3_v1.h"

#define RMR_ANDROID_MAX_INPUT 1048576

JNIEXPORT jboolean JNICALL
Java_io_rafaelia_rmrhash_NativeHash_selfTest(JNIEnv *env, jclass owner) {
    static const rmr_pv1_b3_u8 abc[3] = { 'a', 'b', 'c' };
    static const rmr_pv1_b3_u8 expected[32] = {
        0x64,0x37,0xb3,0xac,0x38,0x46,0x51,0x33,
        0xff,0xb6,0x3b,0x75,0x27,0x3a,0x8d,0xb5,
        0x48,0xc5,0x58,0x46,0x5d,0x79,0xdb,0x03,
        0xfd,0x35,0x9c,0x6c,0xd5,0xbd,0x9d,0x85
    };
    rmr_pv1_b3_u8 digest[32];
    unsigned int different = 0u;
    unsigned int i;
    (void)env; (void)owner;
    if (rmr_pv1_blake3_256(abc, 3u, digest) != 0) return JNI_FALSE;
    for (i = 0u; i < 32u; ++i) different |= (unsigned int)(digest[i] ^ expected[i]);
    return different == 0u ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jbyteArray JNICALL
Java_io_rafaelia_rmrhash_NativeHash_digest(JNIEnv *env, jclass owner, jbyteArray input) {
    jsize length;
    jbyte *bytes = (jbyte *)0;
    rmr_pv1_b3_u8 output[32];
    jbyteArray result;
    int rc;
    (void)owner;
    if (input == (jbyteArray)0) return (jbyteArray)0;
    length = (*env)->GetArrayLength(env, input);
    if (length < 0 || length > RMR_ANDROID_MAX_INPUT) return (jbyteArray)0;

    if (length != 0) {
        bytes = (*env)->GetByteArrayElements(env, input, (jboolean *)0);
        if (bytes == (jbyte *)0) return (jbyteArray)0;
    }
    rc = rmr_pv1_blake3_256((const rmr_pv1_b3_u8 *)bytes,
                             (rmr_pv1_b3_usize)length, output);
    if (bytes != (jbyte *)0)
        (*env)->ReleaseByteArrayElements(env, input, bytes, JNI_ABORT);
    if (rc != 0) return (jbyteArray)0;

    result = (*env)->NewByteArray(env, 32);
    if (result == (jbyteArray)0) return (jbyteArray)0;
    (*env)->SetByteArrayRegion(env, result, 0, 32, (const jbyte *)output);
    return result;
}
