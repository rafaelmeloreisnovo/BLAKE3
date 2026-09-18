public final class RmrFixed256SelfTest {
    private RmrFixed256SelfTest() {}
    public static void main(String[] args) {
        byte[] in = new byte[256];
        byte[] out = new byte[16];
        for (int i=0;i<256;i++) in[i]=(byte)i;
        RmrFixed256.reduce(out,0,in,0);
        /* For input 0..255, each byte position across 16 lanes XORs to 0. */
        int diff=0;
        for (int i=0;i<16;i++) diff |= out[i] & 0xff;
        if (diff != 0) throw new AssertionError("vector-0");

        for (int i=0;i<256;i++) in[i]=0;
        for (int lane=0;lane<16;lane++) in[lane*16 + lane]=(byte)(lane+1);
        RmrFixed256.reduce(out,0,in,0);
        for (int i=0;i<16;i++) {
            if ((out[i]&0xff)!=(i+1)) throw new AssertionError("lane-"+i);
        }
        System.out.println("RMR_FIXED256_JAVA_OK");
    }
}
