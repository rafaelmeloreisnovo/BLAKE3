/*
 * RMR FIXED256 Java low-level mirror.
 * No imports, no third-party dependencies, no allocation in the kernel.
 * This is a deterministic reducer, NOT a cryptographic hash and NOT BLAKE3.
 */
public final class RmrFixed256 {
    public static final int INPUT_BYTES = 256;
    public static final int OUTPUT_BYTES = 16;

    private RmrFixed256() {}

    /*
     * out[outOff..outOff+15] = XOR of the 16 fixed 16-byte lanes in input.
     * Contract: caller supplies valid arrays/ranges. There is no variable-tail path.
     * The JVM still imposes its own runtime/bounds/GC semantics outside this kernel.
     */
    public static void reduce(byte[] out, int outOff, byte[] in, int inOff) {
        int s0=0,s1=0,s2=0,s3=0,s4=0,s5=0,s6=0,s7=0;
        int s8=0,s9=0,s10=0,s11=0,s12=0,s13=0,s14=0,s15=0;
        int p=inOff;
        for (int lane=0; lane<16; lane++, p+=16) {
            s0  ^= in[p];      s1  ^= in[p+1];  s2  ^= in[p+2];  s3  ^= in[p+3];
            s4  ^= in[p+4];    s5  ^= in[p+5];  s6  ^= in[p+6];  s7  ^= in[p+7];
            s8  ^= in[p+8];    s9  ^= in[p+9];  s10 ^= in[p+10]; s11 ^= in[p+11];
            s12 ^= in[p+12];   s13 ^= in[p+13]; s14 ^= in[p+14]; s15 ^= in[p+15];
        }
        out[outOff]    =(byte)s0;  out[outOff+1] =(byte)s1;
        out[outOff+2]  =(byte)s2;  out[outOff+3] =(byte)s3;
        out[outOff+4]  =(byte)s4;  out[outOff+5] =(byte)s5;
        out[outOff+6]  =(byte)s6;  out[outOff+7] =(byte)s7;
        out[outOff+8]  =(byte)s8;  out[outOff+9] =(byte)s9;
        out[outOff+10] =(byte)s10; out[outOff+11]=(byte)s11;
        out[outOff+12] =(byte)s12; out[outOff+13]=(byte)s13;
        out[outOff+14] =(byte)s14; out[outOff+15]=(byte)s15;
    }
}
