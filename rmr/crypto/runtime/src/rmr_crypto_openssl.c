/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * OpenSSL-backed RMR runtime provider.
 * Algorithm != provider != architecture.
 */
#include "rmr_crypto_runtime.h"

#include <limits.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>

int rmr_crypto_blake3_digest(const uint8_t *, size_t, uint8_t *, size_t, size_t *);

static const EVP_MD *rmr_digest_for(rmr_crypto_algorithm algorithm) {
  switch (algorithm) {
    case RMR_CRYPTO_MD5: return EVP_md5();
    case RMR_CRYPTO_SHA1: return EVP_sha1();
    case RMR_CRYPTO_SHA256: return EVP_sha256();
    case RMR_CRYPTO_SHA512: return EVP_sha512();
    default: return NULL;
  }
}

int rmr_crypto_digest(rmr_crypto_algorithm algorithm,
                      const uint8_t *input, size_t input_len,
                      uint8_t *out, size_t out_cap, size_t *out_len) {
  const EVP_MD *md;
  unsigned int n = 0u;
  int md_size;

  if (algorithm == RMR_CRYPTO_BLAKE3) {
    return rmr_crypto_blake3_digest(input, input_len, out, out_cap, out_len);
  }

  md = rmr_digest_for(algorithm);
  if (md == NULL || out == NULL || out_len == NULL ||
      (input == NULL && input_len != 0u)) {
    return -1;
  }

  md_size = EVP_MD_get_size(md);
  if (md_size <= 0 || out_cap < (size_t)md_size) {
    return -1;
  }

  if (EVP_Digest(input, input_len, out, &n, md, NULL) != 1) {
    return -1;
  }

  *out_len = (size_t)n;
  return 0;
}

int rmr_crypto_hmac_sha256(const uint8_t *key, size_t key_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t out[32]) {
  EVP_MAC *mac = NULL;
  EVP_MAC_CTX *ctx = NULL;
  OSSL_PARAM params[2];
  size_t out_len = 0u;
  int ok = -1;

  if (out == NULL || (key == NULL && key_len != 0u) ||
      (input == NULL && input_len != 0u)) {
    return -1;
  }

  mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
  if (mac == NULL) {
    goto done;
  }

  ctx = EVP_MAC_CTX_new(mac);
  if (ctx == NULL) {
    goto done;
  }

  params[0] = OSSL_PARAM_construct_utf8_string(
      OSSL_MAC_PARAM_DIGEST, (char *)"SHA256", 0);
  params[1] = OSSL_PARAM_construct_end();

  if (EVP_MAC_init(ctx, key, key_len, params) != 1) {
    goto done;
  }
  if (input_len != 0u && EVP_MAC_update(ctx, input, input_len) != 1) {
    goto done;
  }
  if (EVP_MAC_final(ctx, out, &out_len, 32u) != 1 || out_len != 32u) {
    goto done;
  }

  ok = 0;

done:
  EVP_MAC_CTX_free(ctx);
  EVP_MAC_free(mac);
  return ok;
}

int rmr_crypto_hkdf_sha256(const uint8_t *ikm, size_t ikm_len,
                           const uint8_t *salt, size_t salt_len,
                           const uint8_t *info, size_t info_len,
                           uint8_t *out, size_t out_len) {
  EVP_KDF *kdf = NULL;
  EVP_KDF_CTX *ctx = NULL;
  OSSL_PARAM params[5];
  int ok = -1;

  if (out == NULL || (ikm == NULL && ikm_len != 0u) ||
      (salt == NULL && salt_len != 0u) ||
      (info == NULL && info_len != 0u)) {
    return -1;
  }

  kdf = EVP_KDF_fetch(NULL, "HKDF", NULL);
  if (kdf == NULL) {
    goto done;
  }

  ctx = EVP_KDF_CTX_new(kdf);
  if (ctx == NULL) {
    goto done;
  }

  params[0] = OSSL_PARAM_construct_utf8_string(
      OSSL_KDF_PARAM_DIGEST, (char *)"SHA256", 0);
  params[1] = OSSL_PARAM_construct_octet_string(
      OSSL_KDF_PARAM_KEY, (void *)ikm, ikm_len);
  params[2] = OSSL_PARAM_construct_octet_string(
      OSSL_KDF_PARAM_SALT, (void *)salt, salt_len);
  params[3] = OSSL_PARAM_construct_octet_string(
      OSSL_KDF_PARAM_INFO, (void *)info, info_len);
  params[4] = OSSL_PARAM_construct_end();

  if (EVP_KDF_derive(ctx, out, out_len, params) != 1) {
    goto done;
  }

  ok = 0;

done:
  EVP_KDF_CTX_free(ctx);
  EVP_KDF_free(kdf);
  return ok;
}

int rmr_crypto_ed25519_public_from_seed(const uint8_t seed[32],
                                        uint8_t public_key[32]) {
  EVP_PKEY *pkey = NULL;
  size_t n = 32u;
  int ok = -1;

  if (seed == NULL || public_key == NULL) {
    return -1;
  }

  pkey = EVP_PKEY_new_raw_private_key(
      EVP_PKEY_ED25519, NULL, seed, 32u);
  if (pkey == NULL) {
    return -1;
  }

  if (EVP_PKEY_get_raw_public_key(pkey, public_key, &n) == 1 && n == 32u) {
    ok = 0;
  }

  EVP_PKEY_free(pkey);
  return ok;
}

int rmr_crypto_ed25519_sign(const uint8_t seed[32],
                            const uint8_t *message, size_t message_len,
                            uint8_t signature[64]) {
  EVP_PKEY *pkey = NULL;
  EVP_MD_CTX *ctx = NULL;
  size_t signature_len = 64u;
  static const uint8_t empty = 0u;
  int ok = -1;

  if (seed == NULL || signature == NULL ||
      (message == NULL && message_len != 0u)) {
    return -1;
  }

  pkey = EVP_PKEY_new_raw_private_key(
      EVP_PKEY_ED25519, NULL, seed, 32u);
  ctx = EVP_MD_CTX_new();
  if (pkey == NULL || ctx == NULL) {
    goto done;
  }

  if (EVP_DigestSignInit(ctx, NULL, NULL, NULL, pkey) != 1) {
    goto done;
  }
  if (EVP_DigestSign(
          ctx,
          signature,
          &signature_len,
          message_len != 0u ? message : &empty,
          message_len) != 1 ||
      signature_len != 64u) {
    goto done;
  }

  ok = 0;

done:
  EVP_MD_CTX_free(ctx);
  EVP_PKEY_free(pkey);
  return ok;
}

int rmr_crypto_ed25519_verify(const uint8_t public_key[32],
                              const uint8_t *message, size_t message_len,
                              const uint8_t signature[64]) {
  EVP_PKEY *pkey = NULL;
  EVP_MD_CTX *ctx = NULL;
  static const uint8_t empty = 0u;
  int rc;

  if (public_key == NULL || signature == NULL ||
      (message == NULL && message_len != 0u)) {
    return -1;
  }

  pkey = EVP_PKEY_new_raw_public_key(
      EVP_PKEY_ED25519, NULL, public_key, 32u);
  ctx = EVP_MD_CTX_new();
  if (pkey == NULL || ctx == NULL) {
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    return -1;
  }

  if (EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, pkey) != 1) {
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    return -1;
  }

  rc = EVP_DigestVerify(
      ctx,
      signature,
      64u,
      message_len != 0u ? message : &empty,
      message_len);

  EVP_MD_CTX_free(ctx);
  EVP_PKEY_free(pkey);

  return rc == 1 ? 0 : (rc == 0 ? 1 : -1);
}

static const EVP_CIPHER *rmr_cipher_for(rmr_crypto_algorithm algorithm) {
  if (algorithm == RMR_CRYPTO_CHACHA20_POLY1305) {
    return EVP_chacha20_poly1305();
  }
  if (algorithm == RMR_CRYPTO_AES_256_GCM) {
    return EVP_aes_256_gcm();
  }
  return NULL;
}

int rmr_crypto_aead_encrypt(rmr_crypto_algorithm algorithm,
                            const uint8_t key[32], const uint8_t nonce[12],
                            const uint8_t *aad, size_t aad_len,
                            const uint8_t *plaintext, size_t plaintext_len,
                            uint8_t *ciphertext, uint8_t tag[16]) {
  EVP_CIPHER_CTX *ctx = NULL;
  const EVP_CIPHER *cipher = rmr_cipher_for(algorithm);
  int n = 0;
  int total = 0;
  int ok = -1;

  if (cipher == NULL || key == NULL || nonce == NULL ||
      ciphertext == NULL || tag == NULL ||
      (aad == NULL && aad_len != 0u) ||
      (plaintext == NULL && plaintext_len != 0u)) {
    return -1;
  }
  if (aad_len > INT_MAX || plaintext_len > INT_MAX) {
    return -1;
  }

  ctx = EVP_CIPHER_CTX_new();
  if (ctx == NULL) {
    return -1;
  }

  if (EVP_EncryptInit_ex(ctx, cipher, NULL, NULL, NULL) != 1) {
    goto done;
  }
  if (EVP_CIPHER_CTX_ctrl(
          ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, NULL) != 1) {
    goto done;
  }
  if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, nonce) != 1) {
    goto done;
  }
  if (aad_len != 0u &&
      EVP_EncryptUpdate(ctx, NULL, &n, aad, (int)aad_len) != 1) {
    goto done;
  }
  if (plaintext_len != 0u &&
      EVP_EncryptUpdate(
          ctx, ciphertext, &n, plaintext, (int)plaintext_len) != 1) {
    goto done;
  }

  total = n;
  if (EVP_EncryptFinal_ex(ctx, ciphertext + total, &n) != 1) {
    goto done;
  }
  total += n;

  if ((size_t)total != plaintext_len) {
    goto done;
  }

  if (EVP_CIPHER_CTX_ctrl(
          ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag) != 1) {
    goto done;
  }

  ok = 0;

done:
  EVP_CIPHER_CTX_free(ctx);
  return ok;
}

int rmr_crypto_aead_decrypt(rmr_crypto_algorithm algorithm,
                            const uint8_t key[32], const uint8_t nonce[12],
                            const uint8_t *aad, size_t aad_len,
                            const uint8_t *ciphertext, size_t ciphertext_len,
                            const uint8_t tag[16], uint8_t *plaintext) {
  EVP_CIPHER_CTX *ctx = NULL;
  const EVP_CIPHER *cipher = rmr_cipher_for(algorithm);
  int n = 0;
  int total = 0;
  int rc;

  if (cipher == NULL || key == NULL || nonce == NULL ||
      plaintext == NULL || tag == NULL ||
      (aad == NULL && aad_len != 0u) ||
      (ciphertext == NULL && ciphertext_len != 0u)) {
    return -1;
  }
  if (aad_len > INT_MAX || ciphertext_len > INT_MAX) {
    return -1;
  }

  ctx = EVP_CIPHER_CTX_new();
  if (ctx == NULL) {
    return -1;
  }

  if (EVP_DecryptInit_ex(ctx, cipher, NULL, NULL, NULL) != 1) {
    goto error;
  }
  if (EVP_CIPHER_CTX_ctrl(
          ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, NULL) != 1) {
    goto error;
  }
  if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, nonce) != 1) {
    goto error;
  }
  if (aad_len != 0u &&
      EVP_DecryptUpdate(ctx, NULL, &n, aad, (int)aad_len) != 1) {
    goto error;
  }
  if (ciphertext_len != 0u &&
      EVP_DecryptUpdate(
          ctx, plaintext, &n, ciphertext, (int)ciphertext_len) != 1) {
    goto error;
  }

  total = n;

  if (EVP_CIPHER_CTX_ctrl(
          ctx, EVP_CTRL_AEAD_SET_TAG, 16, (void *)tag) != 1) {
    goto error;
  }

  rc = EVP_DecryptFinal_ex(ctx, plaintext + total, &n);
  EVP_CIPHER_CTX_free(ctx);

  if (rc != 1) {
    return 1;
  }

  total += n;
  return (size_t)total == ciphertext_len ? 0 : -1;

error:
  EVP_CIPHER_CTX_free(ctx);
  return -1;
}
