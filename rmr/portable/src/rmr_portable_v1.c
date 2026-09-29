/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 *
 * Fixed 256-byte reducer. Statically unrolled: no variable tail and no loop.
 */
#include "../include/rmr_portable_v1.h"

void rmr_pv1_reduce256(rmr_pv1_u8 out[16], const rmr_pv1_u8 in[256]) {
    out[0u] = (rmr_pv1_u8)(in[0u] ^ in[16u] ^ in[32u] ^ in[48u] ^ in[64u] ^ in[80u] ^ in[96u] ^ in[112u] ^ in[128u] ^ in[144u] ^ in[160u] ^ in[176u] ^ in[192u] ^ in[208u] ^ in[224u] ^ in[240u]);
    out[1u] = (rmr_pv1_u8)(in[1u] ^ in[17u] ^ in[33u] ^ in[49u] ^ in[65u] ^ in[81u] ^ in[97u] ^ in[113u] ^ in[129u] ^ in[145u] ^ in[161u] ^ in[177u] ^ in[193u] ^ in[209u] ^ in[225u] ^ in[241u]);
    out[2u] = (rmr_pv1_u8)(in[2u] ^ in[18u] ^ in[34u] ^ in[50u] ^ in[66u] ^ in[82u] ^ in[98u] ^ in[114u] ^ in[130u] ^ in[146u] ^ in[162u] ^ in[178u] ^ in[194u] ^ in[210u] ^ in[226u] ^ in[242u]);
    out[3u] = (rmr_pv1_u8)(in[3u] ^ in[19u] ^ in[35u] ^ in[51u] ^ in[67u] ^ in[83u] ^ in[99u] ^ in[115u] ^ in[131u] ^ in[147u] ^ in[163u] ^ in[179u] ^ in[195u] ^ in[211u] ^ in[227u] ^ in[243u]);
    out[4u] = (rmr_pv1_u8)(in[4u] ^ in[20u] ^ in[36u] ^ in[52u] ^ in[68u] ^ in[84u] ^ in[100u] ^ in[116u] ^ in[132u] ^ in[148u] ^ in[164u] ^ in[180u] ^ in[196u] ^ in[212u] ^ in[228u] ^ in[244u]);
    out[5u] = (rmr_pv1_u8)(in[5u] ^ in[21u] ^ in[37u] ^ in[53u] ^ in[69u] ^ in[85u] ^ in[101u] ^ in[117u] ^ in[133u] ^ in[149u] ^ in[165u] ^ in[181u] ^ in[197u] ^ in[213u] ^ in[229u] ^ in[245u]);
    out[6u] = (rmr_pv1_u8)(in[6u] ^ in[22u] ^ in[38u] ^ in[54u] ^ in[70u] ^ in[86u] ^ in[102u] ^ in[118u] ^ in[134u] ^ in[150u] ^ in[166u] ^ in[182u] ^ in[198u] ^ in[214u] ^ in[230u] ^ in[246u]);
    out[7u] = (rmr_pv1_u8)(in[7u] ^ in[23u] ^ in[39u] ^ in[55u] ^ in[71u] ^ in[87u] ^ in[103u] ^ in[119u] ^ in[135u] ^ in[151u] ^ in[167u] ^ in[183u] ^ in[199u] ^ in[215u] ^ in[231u] ^ in[247u]);
    out[8u] = (rmr_pv1_u8)(in[8u] ^ in[24u] ^ in[40u] ^ in[56u] ^ in[72u] ^ in[88u] ^ in[104u] ^ in[120u] ^ in[136u] ^ in[152u] ^ in[168u] ^ in[184u] ^ in[200u] ^ in[216u] ^ in[232u] ^ in[248u]);
    out[9u] = (rmr_pv1_u8)(in[9u] ^ in[25u] ^ in[41u] ^ in[57u] ^ in[73u] ^ in[89u] ^ in[105u] ^ in[121u] ^ in[137u] ^ in[153u] ^ in[169u] ^ in[185u] ^ in[201u] ^ in[217u] ^ in[233u] ^ in[249u]);
    out[10u] = (rmr_pv1_u8)(in[10u] ^ in[26u] ^ in[42u] ^ in[58u] ^ in[74u] ^ in[90u] ^ in[106u] ^ in[122u] ^ in[138u] ^ in[154u] ^ in[170u] ^ in[186u] ^ in[202u] ^ in[218u] ^ in[234u] ^ in[250u]);
    out[11u] = (rmr_pv1_u8)(in[11u] ^ in[27u] ^ in[43u] ^ in[59u] ^ in[75u] ^ in[91u] ^ in[107u] ^ in[123u] ^ in[139u] ^ in[155u] ^ in[171u] ^ in[187u] ^ in[203u] ^ in[219u] ^ in[235u] ^ in[251u]);
    out[12u] = (rmr_pv1_u8)(in[12u] ^ in[28u] ^ in[44u] ^ in[60u] ^ in[76u] ^ in[92u] ^ in[108u] ^ in[124u] ^ in[140u] ^ in[156u] ^ in[172u] ^ in[188u] ^ in[204u] ^ in[220u] ^ in[236u] ^ in[252u]);
    out[13u] = (rmr_pv1_u8)(in[13u] ^ in[29u] ^ in[45u] ^ in[61u] ^ in[77u] ^ in[93u] ^ in[109u] ^ in[125u] ^ in[141u] ^ in[157u] ^ in[173u] ^ in[189u] ^ in[205u] ^ in[221u] ^ in[237u] ^ in[253u]);
    out[14u] = (rmr_pv1_u8)(in[14u] ^ in[30u] ^ in[46u] ^ in[62u] ^ in[78u] ^ in[94u] ^ in[110u] ^ in[126u] ^ in[142u] ^ in[158u] ^ in[174u] ^ in[190u] ^ in[206u] ^ in[222u] ^ in[238u] ^ in[254u]);
    out[15u] = (rmr_pv1_u8)(in[15u] ^ in[31u] ^ in[47u] ^ in[63u] ^ in[79u] ^ in[95u] ^ in[111u] ^ in[127u] ^ in[143u] ^ in[159u] ^ in[175u] ^ in[191u] ^ in[207u] ^ in[223u] ^ in[239u] ^ in[255u]);
}

void rmr_pv1_batch4(rmr_pv1_u8 out[64], const rmr_pv1_u8 in[1024]) {
    rmr_pv1_reduce256(out + 0u, in + 0u);
    rmr_pv1_reduce256(out + 16u, in + 256u);
    rmr_pv1_reduce256(out + 32u, in + 512u);
    rmr_pv1_reduce256(out + 48u, in + 768u);
}

rmr_pv1_u32 rmr_pv1_equal16(const rmr_pv1_u8 a[16], const rmr_pv1_u8 b[16]) {
    rmr_pv1_u32 d =
        (rmr_pv1_u32)(a[0]^b[0]) | (rmr_pv1_u32)(a[1]^b[1]) |
        (rmr_pv1_u32)(a[2]^b[2]) | (rmr_pv1_u32)(a[3]^b[3]) |
        (rmr_pv1_u32)(a[4]^b[4]) | (rmr_pv1_u32)(a[5]^b[5]) |
        (rmr_pv1_u32)(a[6]^b[6]) | (rmr_pv1_u32)(a[7]^b[7]) |
        (rmr_pv1_u32)(a[8]^b[8]) | (rmr_pv1_u32)(a[9]^b[9]) |
        (rmr_pv1_u32)(a[10]^b[10]) | (rmr_pv1_u32)(a[11]^b[11]) |
        (rmr_pv1_u32)(a[12]^b[12]) | (rmr_pv1_u32)(a[13]^b[13]) |
        (rmr_pv1_u32)(a[14]^b[14]) | (rmr_pv1_u32)(a[15]^b[15]);
    return (rmr_pv1_u32)(d == 0u);
}

rmr_pv1_u32 rmr_pv1_selftest(void) {
    static const rmr_pv1_u8 x[256] = { [0] = 1u, [17] = 2u, [34] = 4u, [51] = 8u };
    static const rmr_pv1_u8 e[16] = { 1u, 2u, 4u, 8u };
    rmr_pv1_u8 y[16];
    rmr_pv1_reduce256(y, x);
    return rmr_pv1_equal16(y, e);
}
