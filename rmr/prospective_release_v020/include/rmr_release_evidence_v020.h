/*
 * Copyright (c) 2026 Rafael Melo Reis — prospective new module; legal title review pending.
 * SPDX-License-Identifier: LicenseRef-RAFAELIA-Proprietary-Candidate-0.2
 * Candidate source notice only. See LICENSE_CANDIDATE.md (NOT AN ACTIVE LICENSE).
 * No code from upstream BLAKE3 is included here.
 */
#ifndef RMR_RELEASE_EVIDENCE_V020_H
#define RMR_RELEASE_EVIDENCE_V020_H
#if !defined(__GNUC__) && !defined(__clang__)
#error "RMR release evidence V0.2 requires GCC or Clang integer types"
#endif
typedef __UINT8_TYPE__ rmr_re_u8;
typedef __UINT32_TYPE__ rmr_re_u32;
#define RMR_RE_PACKET_BYTES 64u
#define RMR_RE_SCHEMA 1u
#define RMR_RE_VERSION_MAJOR 0u
#define RMR_RE_VERSION_MINOR 2u
#define RMR_RE_VERSION_PATCH 0u
#define RMR_RE_VERSION_RC 1u
#define RMR_RE_STATUS_DRAFT 1u
#define RMR_RE_OK 0u
#define RMR_RE_ERR_ARGUMENT 1u
#define RMR_RE_ERR_FORMAT 2u
#define RMR_RE_ERR_CRC 3u
#define RMR_RE_ERR_ZERO_DIGEST 4u
/*
 * Packet layout (64 fixed bytes, little endian for CRC only):
 *   0..3 magic "RMRE"; 4 schema; 5..8 semver 0.2.0-rc.1;
 *   9 status==DRAFT; 10..11 reserved zero; 12..43 caller-provided SHA-256;
 *   44..59 caller-provided 16-byte opaque reference;
 *   60..63 CRC32C(0..59), little endian.
 * CRC32C IS NOT authentication/signature/title proof. A nonzero reference
 * DOES NOT verify a commercial contract, license, identity, or signature.
 * BLAKE3 algorithm/provider is neither implemented nor relicensed here.
 */
rmr_re_u32 rmr_re_make_draft(rmr_re_u8 packet[RMR_RE_PACKET_BYTES],
                              const rmr_re_u8 asset_sha256[32],
                              const rmr_re_u8 reference[16]);
rmr_re_u32 rmr_re_check_format(const rmr_re_u8 packet[RMR_RE_PACKET_BYTES]);
rmr_re_u32 rmr_re_crc32c(const rmr_re_u8 *data, rmr_re_u32 n);
#endif
