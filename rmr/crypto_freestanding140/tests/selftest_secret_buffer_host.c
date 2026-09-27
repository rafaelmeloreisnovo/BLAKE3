/*
 * Hosted witness for the secret-line wipe contract.
 * Test harness may use ordinary control flow; production secret kernel may not.
 */
#include "../include/rmr_cf140_secret_buffer.h"

int main(void) {
    rmr_cf_secret_workspace w;
    unsigned int i;

    for (i = 0u; i < 64u; ++i) {
        w.key.bytes[i] = (rmr_cf_u8)(i + 1u);
        w.work0.bytes[i] = (rmr_cf_u8)(0xa5u ^ i);
        w.work1.bytes[i] = (rmr_cf_u8)(0x5au ^ i);
    }

    rmr_cf_secret_workspace_zero(&w);

    for (i = 0u; i < 64u; ++i) {
        if (w.key.bytes[i] != 0u ||
            w.work0.bytes[i] != 0u ||
            w.work1.bytes[i] != 0u) {
            return 1;
        }
    }

    return 0;
}
