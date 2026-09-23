/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
#include "rmr_crypto_runtime.h"

const char *rmr_crypto_algorithm_name(rmr_crypto_algorithm algorithm) {
  switch (algorithm) {
    case RMR_CRYPTO_BLAKE3: return "BLAKE3";
    case RMR_CRYPTO_MD5: return "MD5";
    case RMR_CRYPTO_SHA1: return "SHA-1";
    case RMR_CRYPTO_SHA256: return "SHA-256";
    case RMR_CRYPTO_SHA512: return "SHA-512";
    case RMR_CRYPTO_HMAC_SHA256: return "HMAC-SHA256";
    case RMR_CRYPTO_HKDF_SHA256: return "HKDF-SHA256";
    case RMR_CRYPTO_ED25519: return "Ed25519";
    case RMR_CRYPTO_CHACHA20_POLY1305: return "ChaCha20-Poly1305";
    case RMR_CRYPTO_AES_256_GCM: return "AES-256-GCM";
    default: return "UNKNOWN";
  }
}

const char *rmr_crypto_provider_name(void) {
  return "openssl+blake3-upstream";
}
