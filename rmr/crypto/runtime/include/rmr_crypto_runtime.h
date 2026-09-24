/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Provider-neutral RMR cryptographic runtime API.
 * BLAKE3 remains upstream; other primitives are delegated to an audited provider.
 */
#ifndef RMR_CRYPTO_RUNTIME_H
#define RMR_CRYPTO_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  RMR_CRYPTO_BLAKE3 = 1,
  RMR_CRYPTO_MD5,
  RMR_CRYPTO_SHA1,
  RMR_CRYPTO_SHA256,
  RMR_CRYPTO_SHA512,
  RMR_CRYPTO_SHA3_256,
  RMR_CRYPTO_BLAKE2B_512,
  RMR_CRYPTO_HMAC_SHA256,
  RMR_CRYPTO_HMAC_SHA512,
  RMR_CRYPTO_HKDF_SHA256,
  RMR_CRYPTO_ED25519,
  RMR_CRYPTO_CHACHA20_POLY1305,
  RMR_CRYPTO_AES_256_GCM,
  RMR_CRYPTO_X25519,
  RMR_CRYPTO_PBKDF2_HMAC_SHA256,
  RMR_CRYPTO_SHA224,
  RMR_CRYPTO_SHA384,
  RMR_CRYPTO_SHA3_512,
  RMR_CRYPTO_SHAKE128,
  RMR_CRYPTO_SHAKE256,
  RMR_CRYPTO_BLAKE2S_256,
  RMR_CRYPTO_ED448,
  RMR_CRYPTO_X448
} rmr_crypto_algorithm;

const char *rmr_crypto_algorithm_name(rmr_crypto_algorithm algorithm);
const char *rmr_crypto_provider_name(void);

int rmr_crypto_digest(rmr_crypto_algorithm algorithm,
                      const uint8_t *input, size_t input_len,
                      uint8_t *out, size_t out_cap, size_t *out_len);

int rmr_crypto_xof(rmr_crypto_algorithm algorithm,
                   const uint8_t *input, size_t input_len,
                   uint8_t *out, size_t out_len);

int rmr_crypto_hmac_sha256(const uint8_t *key, size_t key_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t out[32]);
int rmr_crypto_hmac_sha512(const uint8_t *key, size_t key_len,
                           const uint8_t *input, size_t input_len,
                           uint8_t out[64]);

int rmr_crypto_hkdf_sha256(const uint8_t *ikm, size_t ikm_len,
                           const uint8_t *salt, size_t salt_len,
                           const uint8_t *info, size_t info_len,
                           uint8_t *out, size_t out_len);

int rmr_crypto_ed25519_public_from_seed(const uint8_t seed[32],
                                        uint8_t public_key[32]);
int rmr_crypto_ed25519_sign(const uint8_t seed[32],
                            const uint8_t *message, size_t message_len,
                            uint8_t signature[64]);
int rmr_crypto_ed25519_verify(const uint8_t public_key[32],
                              const uint8_t *message, size_t message_len,
                              const uint8_t signature[64]);

int rmr_crypto_ed448_public_from_seed(const uint8_t seed[57],
                                      uint8_t public_key[57]);
int rmr_crypto_ed448_sign(const uint8_t seed[57],
                          const uint8_t *message, size_t message_len,
                          uint8_t signature[114]);
int rmr_crypto_ed448_verify(const uint8_t public_key[57],
                            const uint8_t *message, size_t message_len,
                            const uint8_t signature[114]);

int rmr_crypto_x25519_public_from_private(const uint8_t private_key[32],
                                          uint8_t public_key[32]);
int rmr_crypto_x25519_shared_secret(const uint8_t private_key[32],
                                    const uint8_t peer_public_key[32],
                                    uint8_t shared_secret[32]);

int rmr_crypto_x448_public_from_private(const uint8_t private_key[56],
                                        uint8_t public_key[56]);
int rmr_crypto_x448_shared_secret(const uint8_t private_key[56],
                                  const uint8_t peer_public_key[56],
                                  uint8_t shared_secret[56]);

int rmr_crypto_pbkdf2_hmac_sha256(const uint8_t *password, size_t password_len,
                                  const uint8_t *salt, size_t salt_len,
                                  uint32_t iterations,
                                  uint8_t *out, size_t out_len);

int rmr_crypto_aead_encrypt(rmr_crypto_algorithm algorithm,
                            const uint8_t key[32], const uint8_t nonce[12],
                            const uint8_t *aad, size_t aad_len,
                            const uint8_t *plaintext, size_t plaintext_len,
                            uint8_t *ciphertext, uint8_t tag[16]);

int rmr_crypto_aead_decrypt(rmr_crypto_algorithm algorithm,
                            const uint8_t key[32], const uint8_t nonce[12],
                            const uint8_t *aad, size_t aad_len,
                            const uint8_t *ciphertext, size_t ciphertext_len,
                            const uint8_t tag[16], uint8_t *plaintext);

#ifdef __cplusplus
}
#endif

#endif
