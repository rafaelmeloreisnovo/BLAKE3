/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
#include "rmr_crypto_runtime.h"

#include <stdio.h>
#include <string.h>

static int hex_to_bytes(const char *hex, uint8_t *out, size_t n) {
  size_t i;
  unsigned value;

  for (i = 0u; i < n; ++i) {
    if (sscanf(hex + (2u * i), "%2x", &value) != 1) {
      return -1;
    }
    out[i] = (uint8_t)value;
  }

  return hex[2u * n] == '\0' ? 0 : -1;
}

static int bytes_equal(const uint8_t *lhs, const uint8_t *rhs, size_t n) {
  unsigned diff = 0u;
  size_t i;

  for (i = 0u; i < n; ++i) {
    diff |= (unsigned)(lhs[i] ^ rhs[i]);
  }

  return diff == 0u;
}

static int digest_kat(rmr_crypto_algorithm algorithm,
                      const char *expected_hex,
                      size_t expected_len) {
  uint8_t out[64];
  uint8_t expected[64];
  size_t out_len = 0u;

  if (hex_to_bytes(expected_hex, expected, expected_len) != 0) {
    return -1;
  }

  if (rmr_crypto_digest(
          algorithm,
          (const uint8_t *)"abc",
          3u,
          out,
          sizeof(out),
          &out_len) != 0 ||
      out_len != expected_len ||
      !bytes_equal(out, expected, expected_len)) {
    return -1;
  }

  return 0;
}

int main(void) {
  uint8_t out[256];
  uint8_t expected[256];
  uint8_t public_key[32];
  uint8_t signature[64];
  uint8_t tag[16];
  uint8_t ciphertext[128];
  uint8_t plaintext_out[128];
  uint8_t hmac_key[20];

  memset(hmac_key, 0x0b, sizeof(hmac_key));

  if (digest_kat(
          RMR_CRYPTO_BLAKE3,
          "6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85",
          32u) != 0) {
    return 1;
  }
  if (digest_kat(
          RMR_CRYPTO_MD5,
          "900150983cd24fb0d6963f7d28e17f72",
          16u) != 0) {
    return 2;
  }
  if (digest_kat(
          RMR_CRYPTO_SHA1,
          "a9993e364706816aba3e25717850c26c9cd0d89d",
          20u) != 0) {
    return 3;
  }
  if (digest_kat(
          RMR_CRYPTO_SHA256,
          "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
          32u) != 0) {
    return 4;
  }
  if (digest_kat(
          RMR_CRYPTO_SHA512,
          "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
          "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
          64u) != 0) {
    return 5;
  }
  if (digest_kat(
          RMR_CRYPTO_SHA3_256,
          "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532",
          32u) != 0) {
    return 25;
  }
  if (digest_kat(
          RMR_CRYPTO_BLAKE2B_512,
          "ba80a53f981c4d0d6a2797b69f12f6e"
          "94c212f14685ac4b74b12bb6fdbffa2d"
          "17d87c5392aab792dc252d5de4533cc9"
          "518d38aa8dbf1925ab92386edd4009923",
          64u) != 0) {
    return 26;
  }

  if (rmr_crypto_hmac_sha256(
          hmac_key,
          sizeof(hmac_key),
          (const uint8_t *)"Hi There",
          8u,
          out) != 0) {
    return 6;
  }
  if (hex_to_bytes(
          "b0344c61d8db38535ca8afceaf0bf12b"
          "881dc200c9833da726e9376c2e32cff7",
          expected,
          32u) != 0 ||
      !bytes_equal(out, expected, 32u)) {
    return 7;
  }

  {
    uint8_t ikm[22];
    uint8_t salt[13];
    uint8_t info[10];
    size_t i;

    memset(ikm, 0x0b, sizeof(ikm));
    for (i = 0u; i < sizeof(salt); ++i) {
      salt[i] = (uint8_t)i;
    }
    for (i = 0u; i < sizeof(info); ++i) {
      info[i] = (uint8_t)(0xf0u + i);
    }

    if (rmr_crypto_hkdf_sha256(
            ikm,
            sizeof(ikm),
            salt,
            sizeof(salt),
            info,
            sizeof(info),
            out,
            42u) != 0) {
      return 8;
    }

    if (hex_to_bytes(
            "3cb25f25faacd57a90434f64d0362f2a"
            "2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
            "34007208d5b887185865",
            expected,
            42u) != 0 ||
        !bytes_equal(out, expected, 42u)) {
      return 9;
    }
  }

  {
    uint8_t seed[32];

    if (hex_to_bytes(
            "9d61b19deffd5a60ba844af492ec2cc4"
            "4449c5697b326919703bac031cae7f60",
            seed,
            32u) != 0) {
      return 10;
    }

    if (rmr_crypto_ed25519_public_from_seed(seed, public_key) != 0) {
      return 11;
    }
    if (hex_to_bytes(
            "d75a980182b10ab7d54bfed3c964073a"
            "0ee172f3daa62325af021a68f707511a",
            expected,
            32u) != 0 ||
        !bytes_equal(public_key, expected, 32u)) {
      return 12;
    }

    if (rmr_crypto_ed25519_sign(seed, NULL, 0u, signature) != 0) {
      return 13;
    }
    if (hex_to_bytes(
            "e5564300c360ac729086e2cc806e828a"
            "84877f1eb8e5d974d873e06522490155"
            "5fb8821590a33bacc61e39701cf9b46b"
            "d25bf5f0595bbe24655141438e7a100b",
            expected,
            64u) != 0 ||
        !bytes_equal(signature, expected, 64u)) {
      return 14;
    }

    if (rmr_crypto_ed25519_verify(
            public_key, NULL, 0u, signature) != 0) {
      return 15;
    }

    signature[0] ^= 1u;
    if (rmr_crypto_ed25519_verify(
            public_key, NULL, 0u, signature) != 1) {
      return 16;
    }
  }

  {
    uint8_t key[32] = {0};
    uint8_t nonce[12] = {0};
    uint8_t plaintext[16] = {0};

    if (rmr_crypto_aead_encrypt(
            RMR_CRYPTO_AES_256_GCM,
            key,
            nonce,
            NULL,
            0u,
            plaintext,
            sizeof(plaintext),
            ciphertext,
            tag) != 0) {
      return 17;
    }

    if (hex_to_bytes(
            "cea7403d4d606b6e074ec5d3baf39d18",
            expected,
            16u) != 0 ||
        !bytes_equal(ciphertext, expected, 16u)) {
      return 18;
    }
    if (hex_to_bytes(
            "d0d1c8a799996bf0265b98b5d48ab919",
            expected,
            16u) != 0 ||
        !bytes_equal(tag, expected, 16u)) {
      return 19;
    }

    if (rmr_crypto_aead_decrypt(
            RMR_CRYPTO_AES_256_GCM,
            key,
            nonce,
            NULL,
            0u,
            ciphertext,
            16u,
            tag,
            plaintext_out) != 0 ||
        !bytes_equal(plaintext_out, plaintext, 16u)) {
      return 20;
    }
  }

  {
    uint8_t key[32];
    uint8_t nonce[12];
    uint8_t aad[12];
    static const uint8_t plaintext[] =
        "Ladies and Gentlemen of the class of '99: If I could offer you only "
        "one tip for the future, sunscreen would be it.";
    size_t i;

    for (i = 0u; i < sizeof(key); ++i) {
      key[i] = (uint8_t)(0x80u + i);
    }
    nonce[0] = 0x07u;
    nonce[1] = 0u;
    nonce[2] = 0u;
    nonce[3] = 0u;
    for (i = 0u; i < 8u; ++i) {
      nonce[4u + i] = (uint8_t)(0x40u + i);
    }

    aad[0] = 0x50u;
    aad[1] = 0x51u;
    aad[2] = 0x52u;
    aad[3] = 0x53u;
    for (i = 0u; i < 8u; ++i) {
      aad[4u + i] = (uint8_t)(0xc0u + i);
    }

    if (rmr_crypto_aead_encrypt(
            RMR_CRYPTO_CHACHA20_POLY1305,
            key,
            nonce,
            aad,
            sizeof(aad),
            plaintext,
            sizeof(plaintext) - 1u,
            ciphertext,
            tag) != 0) {
      return 21;
    }

    if (hex_to_bytes(
            "d31a8d34648e60db7b86afbc53ef7ec2"
            "a4aded51296e08fea9e2b5a736ee62d6"
            "3dbea45e8ca9671282fafb69da92728b"
            "1a71de0a9e060b2905d6a5b67ecd3b36"
            "92ddbd7f2d778b8c9803aee328091b58"
            "fab324e4fad675945585808b4831d7bc"
            "3ff4def08e4b7a9de576d26586cec64b"
            "6116",
            expected,
            sizeof(plaintext) - 1u) != 0 ||
        !bytes_equal(
            ciphertext, expected, sizeof(plaintext) - 1u)) {
      return 22;
    }

    if (hex_to_bytes(
            "1ae10b594f09e26a7e902ecbd0600691",
            expected,
            16u) != 0 ||
        !bytes_equal(tag, expected, 16u)) {
      return 23;
    }

    if (rmr_crypto_aead_decrypt(
            RMR_CRYPTO_CHACHA20_POLY1305,
            key,
            nonce,
            aad,
            sizeof(aad),
            ciphertext,
            sizeof(plaintext) - 1u,
            tag,
            plaintext_out) != 0 ||
        !bytes_equal(
            plaintext_out, plaintext, sizeof(plaintext) - 1u)) {
      return 24;
    }
  }

  {
    if (rmr_crypto_hmac_sha512(
            hmac_key,
            sizeof(hmac_key),
            (const uint8_t *)"Hi There",
            8u,
            out) != 0) {
      return 27;
    }
    if (hex_to_bytes(
            "87aa7cdea5ef619d4ff0b4241a1d6cb0"
            "2379f4e2ce4ec2787ad0b30545e17cde"
            "daa833b7d6b8a702038b274eaea3f4e4"
            "be9d914eeb61f1702e696c203a126854",
            expected,
            64u) != 0 ||
        !bytes_equal(out, expected, 64u)) {
      return 28;
    }
  }

  {
    uint8_t alice_private[32];
    uint8_t alice_public[32];
    uint8_t bob_private[32];
    uint8_t bob_public[32];
    uint8_t shared_a[32];
    uint8_t shared_b[32];

    if (hex_to_bytes(
            "77076d0a7318a57d3c16c17251b26645"
            "df4c2f87ebc0992ab177fba51db92c2a",
            alice_private,
            32u) != 0 ||
        hex_to_bytes(
            "5dab087e624a8a4b79e17f8b83800ee6"
            "6f3bb1292618b6fd1c2f8b27ff88e0eb",
            bob_private,
            32u) != 0) {
      return 29;
    }

    if (rmr_crypto_x25519_public_from_private(
            alice_private, alice_public) != 0 ||
        rmr_crypto_x25519_public_from_private(
            bob_private, bob_public) != 0) {
      return 30;
    }

    if (hex_to_bytes(
            "8520f0098930a754748b7ddcb43ef75a"
            "0dbf3a0d26381af4eba4a98eaa9b4e6a",
            expected,
            32u) != 0 ||
        !bytes_equal(alice_public, expected, 32u)) {
      return 31;
    }

    if (hex_to_bytes(
            "de9edb7d7b7dc1b4d35b61c2ece43537"
            "3f8343c85b78674dadfc7e146f882b4f",
            expected,
            32u) != 0 ||
        !bytes_equal(bob_public, expected, 32u)) {
      return 32;
    }

    if (rmr_crypto_x25519_shared_secret(
            alice_private, bob_public, shared_a) != 0 ||
        rmr_crypto_x25519_shared_secret(
            bob_private, alice_public, shared_b) != 0 ||
        !bytes_equal(shared_a, shared_b, 32u)) {
      return 33;
    }

    if (hex_to_bytes(
            "4a5d9d5ba4ce2de1728e3bf480350f25"
            "e07e21c947d19e3376f09b3c1e161742",
            expected,
            32u) != 0 ||
        !bytes_equal(shared_a, expected, 32u)) {
      return 34;
    }
  }

  {
    if (rmr_crypto_pbkdf2_hmac_sha256(
            (const uint8_t *)"password",
            8u,
            (const uint8_t *)"salt",
            4u,
            1u,
            out,
            32u) != 0) {
      return 35;
    }

    if (hex_to_bytes(
            "120fb6cffcf8b32c43e7225256c4f837"
            "a86548c92ccc35480805987cb70be17b",
            expected,
            32u) != 0 ||
        !bytes_equal(out, expected, 32u)) {
      return 36;
    }
  }

  puts("RMR_CRYPTO_RUNTIME_KAT=PASS");
  puts(rmr_crypto_provider_name());
  return 0;
}
