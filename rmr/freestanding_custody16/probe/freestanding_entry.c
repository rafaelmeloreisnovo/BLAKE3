/* Link witness: no syscall, no libc, no heap. */
#include "../include/rmr_fc16.h"
static volatile rmr_fc16_u32 result;
__attribute__((noreturn,used,visibility("default"))) void rmr_fc16_probe_entry(void){
    static const rmr_fc16_u8 p[]={'p'};static const rmr_fc16_u8 d[16]={0};rmr_fc16_context c;rmr_fc16_snapshot s;rmr_fc16_init(&c,0u,0u,0u,0u);result=rmr_fc16_entry_begin(&c,p,1u,16u,RMR_FC16_ENTRY_REGULAR);result|=rmr_fc16_entry_data(&c,d,16u);result|=rmr_fc16_entry_end(&c);result|=rmr_fc16_finalize(&c,0u,&s);result^=s.root_crc32c;for(;;)__asm__ __volatile__("":::"memory");
}
