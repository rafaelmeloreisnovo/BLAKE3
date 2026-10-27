/*
 * Copyright (c) 2026 Rafael Melo Reis — prospective new module; legal title review pending.
 * SPDX-License-Identifier: LicenseRef-RAFAELIA-Proprietary-Candidate-0.2
 * No BLAKE3 implementation, heap, libc, syscall, JNI, or external call.
 */
#include "../include/rmr_release_evidence_v020.h"

rmr_re_u32 rmr_re_crc32c(const rmr_re_u8 *data, rmr_re_u32 n) {
    rmr_re_u32 crc = 0xffffffffu;
    rmr_re_u32 i;
    if (data == (const rmr_re_u8 *)0 && n != 0u) return 0u;
    for (i = 0u; i < n; ++i) {
        rmr_re_u32 j;
        crc ^= (rmr_re_u32)data[i];
        for (j = 0u; j < 8u; ++j) {
            rmr_re_u32 mask = 0u - (crc & 1u);
            crc = (crc >> 1u) ^ (0x82f63b78u & mask);
        }
    }
    return ~crc;
}

static rmr_re_u32 load32le(const rmr_re_u8 *p) {
    return ((rmr_re_u32)p[0]) | (((rmr_re_u32)p[1]) << 8u) |
           (((rmr_re_u32)p[2]) << 16u) | (((rmr_re_u32)p[3]) << 24u);
}

rmr_re_u32 rmr_re_make_draft(rmr_re_u8 packet[RMR_RE_PACKET_BYTES],
                              const rmr_re_u8 asset_sha256[32],
                              const rmr_re_u8 reference[16]) {
    rmr_re_u32 i, nonzero = 0u, crc;
    if (!packet || !asset_sha256 || !reference) return RMR_RE_ERR_ARGUMENT;
    for (i = 0u; i < 32u; ++i) nonzero |= asset_sha256[i];
    if (nonzero == 0u) return RMR_RE_ERR_ZERO_DIGEST;
    packet[0] = 'R'; packet[1] = 'M'; packet[2] = 'R'; packet[3] = 'E';
    packet[4] = (rmr_re_u8)RMR_RE_SCHEMA;
    packet[5] = (rmr_re_u8)RMR_RE_VERSION_MAJOR;
    packet[6] = (rmr_re_u8)RMR_RE_VERSION_MINOR;
    packet[7] = (rmr_re_u8)RMR_RE_VERSION_PATCH;
    packet[8] = (rmr_re_u8)RMR_RE_VERSION_RC;
    packet[9] = (rmr_re_u8)RMR_RE_STATUS_DRAFT;
    packet[10] = 0u; packet[11] = 0u;
    for (i = 0u; i < 32u; ++i) packet[12u + i] = asset_sha256[i];
    for (i = 0u; i < 16u; ++i) packet[44u + i] = reference[i];
    crc = rmr_re_crc32c(packet, 60u);
    packet[60] = (rmr_re_u8)(crc);
    packet[61] = (rmr_re_u8)(crc >> 8u);
    packet[62] = (rmr_re_u8)(crc >> 16u);
    packet[63] = (rmr_re_u8)(crc >> 24u);
    return RMR_RE_OK;
}

rmr_re_u32 rmr_re_check_format(const rmr_re_u8 packet[RMR_RE_PACKET_BYTES]) {
    rmr_re_u32 i, nonzero = 0u;
    if (!packet) return RMR_RE_ERR_ARGUMENT;
    if (packet[0] != 'R' || packet[1] != 'M' ||
        packet[2] != 'R' || packet[3] != 'E' ||
        packet[4] != RMR_RE_SCHEMA ||
        packet[5] != RMR_RE_VERSION_MAJOR ||
        packet[6] != RMR_RE_VERSION_MINOR ||
        packet[7] != RMR_RE_VERSION_PATCH ||
        packet[8] != RMR_RE_VERSION_RC ||
        packet[9] != RMR_RE_STATUS_DRAFT ||
        packet[10] != 0u || packet[11] != 0u) return RMR_RE_ERR_FORMAT;
    for (i = 12u; i < 44u; ++i) nonzero |= packet[i];
    if (nonzero == 0u) return RMR_RE_ERR_ZERO_DIGEST;
    if (load32le(packet + 60u) != rmr_re_crc32c(packet, 60u)) return RMR_RE_ERR_CRC;
    return RMR_RE_OK;
}
