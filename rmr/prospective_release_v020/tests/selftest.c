/* Hosted test harness only. Pure core itself needs no libc. */
#include "../include/rmr_release_evidence_v020.h"
#define CHECK(expr, code) do { if (!(expr)) return (code); } while (0)
int main(void) {
    static const rmr_re_u8 kat[9] = {'1','2','3','4','5','6','7','8','9'};
    rmr_re_u8 packet[64], original[64], sha[32], ref[16], zeros[32] = {0};
    rmr_re_u32 i;
    CHECK(rmr_re_crc32c(kat, 9u) == 0xe3069283u, 1);
    CHECK(rmr_re_crc32c((const rmr_re_u8 *)0, 1u) == 0u, 2);
    for (i=0u;i<32u;i++) sha[i] = (rmr_re_u8)(i + 1u);
    for (i=0u;i<16u;i++) ref[i] = (rmr_re_u8)(0xa0u + i);
    CHECK(rmr_re_make_draft(packet, sha, ref) == RMR_RE_OK, 3);
    CHECK(rmr_re_check_format(packet) == RMR_RE_OK, 4);
    for (i=0u;i<64u;i++) original[i] = packet[i];
    CHECK(rmr_re_make_draft((rmr_re_u8 *)0,sha,ref)==RMR_RE_ERR_ARGUMENT,5);
    CHECK(rmr_re_make_draft(packet,zeros,ref)==RMR_RE_ERR_ZERO_DIGEST,6);
    CHECK(rmr_re_check_format((const rmr_re_u8 *)0)==RMR_RE_ERR_ARGUMENT,7);
    CHECK(rmr_re_check_format(original)==RMR_RE_OK,8);
    original[12] ^= 0x1u;
    CHECK(rmr_re_check_format(original)==RMR_RE_ERR_CRC,9);
    original[12] ^= 0x1u;
    original[9] = 2u; /* NEVER silently call a draft a signed contract. */
    CHECK(rmr_re_check_format(original)==RMR_RE_ERR_FORMAT,10);
    original[9] = 1u;
    original[10] = 1u;
    CHECK(rmr_re_check_format(original)==RMR_RE_ERR_FORMAT,11);
    original[10] = 0u;
    original[8] = 2u; /* no out-of-band version promotion */
    CHECK(rmr_re_check_format(original)==RMR_RE_ERR_FORMAT,12);
    original[8] = 1u;
    CHECK(rmr_re_check_format(original)==RMR_RE_OK,13);
    return 0;
}
