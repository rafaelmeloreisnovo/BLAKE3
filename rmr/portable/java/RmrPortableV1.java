/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 *
 * Java language mirror for the fixed 256-byte reducer only.
 * No import, I/O, allocation inside the kernel, JNI, Android API or reflection.
 * The JVM itself remains an external runtime.
 */
public final class RmrPortableV1 {
    public static final int INPUT_BYTES = 256;
    public static final int OUTPUT_BYTES = 16;

    private RmrPortableV1() {}

    public static void reduce256(byte[] o, int oo, byte[] x, int xo) {
        o[oo+0] = (byte)(x[xo+0] ^ x[xo+16] ^ x[xo+32] ^ x[xo+48] ^ x[xo+64] ^ x[xo+80] ^ x[xo+96] ^ x[xo+112] ^ x[xo+128] ^ x[xo+144] ^ x[xo+160] ^ x[xo+176] ^ x[xo+192] ^ x[xo+208] ^ x[xo+224] ^ x[xo+240]);
        o[oo+1] = (byte)(x[xo+1] ^ x[xo+17] ^ x[xo+33] ^ x[xo+49] ^ x[xo+65] ^ x[xo+81] ^ x[xo+97] ^ x[xo+113] ^ x[xo+129] ^ x[xo+145] ^ x[xo+161] ^ x[xo+177] ^ x[xo+193] ^ x[xo+209] ^ x[xo+225] ^ x[xo+241]);
        o[oo+2] = (byte)(x[xo+2] ^ x[xo+18] ^ x[xo+34] ^ x[xo+50] ^ x[xo+66] ^ x[xo+82] ^ x[xo+98] ^ x[xo+114] ^ x[xo+130] ^ x[xo+146] ^ x[xo+162] ^ x[xo+178] ^ x[xo+194] ^ x[xo+210] ^ x[xo+226] ^ x[xo+242]);
        o[oo+3] = (byte)(x[xo+3] ^ x[xo+19] ^ x[xo+35] ^ x[xo+51] ^ x[xo+67] ^ x[xo+83] ^ x[xo+99] ^ x[xo+115] ^ x[xo+131] ^ x[xo+147] ^ x[xo+163] ^ x[xo+179] ^ x[xo+195] ^ x[xo+211] ^ x[xo+227] ^ x[xo+243]);
        o[oo+4] = (byte)(x[xo+4] ^ x[xo+20] ^ x[xo+36] ^ x[xo+52] ^ x[xo+68] ^ x[xo+84] ^ x[xo+100] ^ x[xo+116] ^ x[xo+132] ^ x[xo+148] ^ x[xo+164] ^ x[xo+180] ^ x[xo+196] ^ x[xo+212] ^ x[xo+228] ^ x[xo+244]);
        o[oo+5] = (byte)(x[xo+5] ^ x[xo+21] ^ x[xo+37] ^ x[xo+53] ^ x[xo+69] ^ x[xo+85] ^ x[xo+101] ^ x[xo+117] ^ x[xo+133] ^ x[xo+149] ^ x[xo+165] ^ x[xo+181] ^ x[xo+197] ^ x[xo+213] ^ x[xo+229] ^ x[xo+245]);
        o[oo+6] = (byte)(x[xo+6] ^ x[xo+22] ^ x[xo+38] ^ x[xo+54] ^ x[xo+70] ^ x[xo+86] ^ x[xo+102] ^ x[xo+118] ^ x[xo+134] ^ x[xo+150] ^ x[xo+166] ^ x[xo+182] ^ x[xo+198] ^ x[xo+214] ^ x[xo+230] ^ x[xo+246]);
        o[oo+7] = (byte)(x[xo+7] ^ x[xo+23] ^ x[xo+39] ^ x[xo+55] ^ x[xo+71] ^ x[xo+87] ^ x[xo+103] ^ x[xo+119] ^ x[xo+135] ^ x[xo+151] ^ x[xo+167] ^ x[xo+183] ^ x[xo+199] ^ x[xo+215] ^ x[xo+231] ^ x[xo+247]);
        o[oo+8] = (byte)(x[xo+8] ^ x[xo+24] ^ x[xo+40] ^ x[xo+56] ^ x[xo+72] ^ x[xo+88] ^ x[xo+104] ^ x[xo+120] ^ x[xo+136] ^ x[xo+152] ^ x[xo+168] ^ x[xo+184] ^ x[xo+200] ^ x[xo+216] ^ x[xo+232] ^ x[xo+248]);
        o[oo+9] = (byte)(x[xo+9] ^ x[xo+25] ^ x[xo+41] ^ x[xo+57] ^ x[xo+73] ^ x[xo+89] ^ x[xo+105] ^ x[xo+121] ^ x[xo+137] ^ x[xo+153] ^ x[xo+169] ^ x[xo+185] ^ x[xo+201] ^ x[xo+217] ^ x[xo+233] ^ x[xo+249]);
        o[oo+10] = (byte)(x[xo+10] ^ x[xo+26] ^ x[xo+42] ^ x[xo+58] ^ x[xo+74] ^ x[xo+90] ^ x[xo+106] ^ x[xo+122] ^ x[xo+138] ^ x[xo+154] ^ x[xo+170] ^ x[xo+186] ^ x[xo+202] ^ x[xo+218] ^ x[xo+234] ^ x[xo+250]);
        o[oo+11] = (byte)(x[xo+11] ^ x[xo+27] ^ x[xo+43] ^ x[xo+59] ^ x[xo+75] ^ x[xo+91] ^ x[xo+107] ^ x[xo+123] ^ x[xo+139] ^ x[xo+155] ^ x[xo+171] ^ x[xo+187] ^ x[xo+203] ^ x[xo+219] ^ x[xo+235] ^ x[xo+251]);
        o[oo+12] = (byte)(x[xo+12] ^ x[xo+28] ^ x[xo+44] ^ x[xo+60] ^ x[xo+76] ^ x[xo+92] ^ x[xo+108] ^ x[xo+124] ^ x[xo+140] ^ x[xo+156] ^ x[xo+172] ^ x[xo+188] ^ x[xo+204] ^ x[xo+220] ^ x[xo+236] ^ x[xo+252]);
        o[oo+13] = (byte)(x[xo+13] ^ x[xo+29] ^ x[xo+45] ^ x[xo+61] ^ x[xo+77] ^ x[xo+93] ^ x[xo+109] ^ x[xo+125] ^ x[xo+141] ^ x[xo+157] ^ x[xo+173] ^ x[xo+189] ^ x[xo+205] ^ x[xo+221] ^ x[xo+237] ^ x[xo+253]);
        o[oo+14] = (byte)(x[xo+14] ^ x[xo+30] ^ x[xo+46] ^ x[xo+62] ^ x[xo+78] ^ x[xo+94] ^ x[xo+110] ^ x[xo+126] ^ x[xo+142] ^ x[xo+158] ^ x[xo+174] ^ x[xo+190] ^ x[xo+206] ^ x[xo+222] ^ x[xo+238] ^ x[xo+254]);
        o[oo+15] = (byte)(x[xo+15] ^ x[xo+31] ^ x[xo+47] ^ x[xo+63] ^ x[xo+79] ^ x[xo+95] ^ x[xo+111] ^ x[xo+127] ^ x[xo+143] ^ x[xo+159] ^ x[xo+175] ^ x[xo+191] ^ x[xo+207] ^ x[xo+223] ^ x[xo+239] ^ x[xo+255]);
    }
}
