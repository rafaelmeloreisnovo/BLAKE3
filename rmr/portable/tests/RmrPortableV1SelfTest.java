/*
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 */
public final class RmrPortableV1SelfTest {
    private RmrPortableV1SelfTest() {}

    public static void main(String[] args) {
        byte[] x = new byte[256];
        byte[] o = new byte[16];
        x[0] = 1;
        x[17] = 2;
        x[34] = 4;
        x[51] = 8;
        RmrPortableV1.reduce256(o, 0, x, 0);
        int d = (o[0] ^ 1) | (o[1] ^ 2) | (o[2] ^ 4) | (o[3] ^ 8);
        for (int i = 4; i < 16; ++i) d |= o[i];
        if (d != 0) throw new AssertionError("RMR_PORTABLE_JAVA_VECTOR");
        System.out.println("RMR_PORTABLE_JAVA_OK");
    }
}
