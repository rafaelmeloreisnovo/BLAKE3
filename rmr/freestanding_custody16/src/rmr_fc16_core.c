/* Copyright (c) 2024-2026 Rafael Melo Reis. Licensed under rmr/LICENSE_RMR. */
#include "../include/rmr_fc16.h"

static void zero_bytes(void *ptr, rmr_fc16_usize n){rmr_fc16_u8 *p=(rmr_fc16_u8*)ptr;while(n--)*p++=0u;}
static void put_u32(rmr_fc16_u8 out[4],rmr_fc16_u32 v){out[0]=(rmr_fc16_u8)v;out[1]=(rmr_fc16_u8)(v>>8u);out[2]=(rmr_fc16_u8)(v>>16u);out[3]=(rmr_fc16_u8)(v>>24u);}
static void put_u64(rmr_fc16_u8 out[8],rmr_fc16_u64 v){for(rmr_fc16_u32 i=0u;i<8u;++i)out[i]=(rmr_fc16_u8)(v>>(i*8u));}
static void feed_meta(rmr_fc16_context *c,const rmr_fc16_u8 *p,rmr_fc16_usize n){c->root_crc_state=rmr_fc16_crc32c_update(c->root_crc_state,p,n);rmr_fc16_sha256_update(&c->sha,p,n);}

void rmr_fc16_init(rmr_fc16_context *c,rmr_fc16_u64 hw,rmr_fc16_u32 tb,rmr_fc16_u32 limit,rmr_fc16_u32 delta){
    if(!c)return;zero_bytes(c,sizeof(*c));c->root_crc_state=0xffffffffu;c->entry_crc_state=0xffffffffu;c->hw_stamp=hw;c->thermal_before_milli_c=tb;c->thermal_limit_milli_c=limit;c->thermal_max_delta_milli_c=delta;c->flags=rmr_fc16_crc32c_backend_flags()|rmr_fc16_vector_backend_flags();if(hw)c->flags|=RMR_FC16_FLAG_HW_STAMP_VALID;for(rmr_fc16_u32 i=0u;i<RMR_FC16_LANES;++i)c->lane[i].crc_state=0xffffffffu;rmr_fc16_sha256_init(&c->sha);static const rmr_fc16_u8 tag[8]={'R','F','C','1','6',0,1,0};feed_meta(c,tag,8u);
}

rmr_fc16_u32 rmr_fc16_entry_begin(rmr_fc16_context *c,const rmr_fc16_u8 *path,rmr_fc16_u32 path_len,rmr_fc16_u64 size,rmr_fc16_u32 kind){
    if(!c||!path||path_len==0u)return RMR_FC16_ERR_ARGUMENT;if(c->entry_open)return RMR_FC16_ERR_STATE;
    rmr_fc16_u32 ps=rmr_fc16_crc32c_update(0xffffffffu,path,path_len);c->path_crc_final=rmr_fc16_crc32c_finish(ps);c->entry_base_lane=(rmr_fc16_u8)(c->path_crc_final&15u);c->entry_crc_state=0xffffffffu;c->entry_declared_size=size;c->entry_seen_size=0u;c->entry_open=1u;
    rmr_fc16_u8 meta[20];meta[0]='E';meta[1]='N';meta[2]='T';meta[3]='R';put_u32(meta+4,path_len);put_u64(meta+8,size);put_u32(meta+16,kind);feed_meta(c,meta,20u);feed_meta(c,path,path_len);
    rmr_fc16_lane *l=&c->lane[c->entry_base_lane];l->crc_state=rmr_fc16_crc32c_update(l->crc_state,meta,20u);l->crc_state=rmr_fc16_crc32c_update(l->crc_state,path,path_len);l->xor_fold^=c->path_crc_final;
    return RMR_FC16_OK;
}

rmr_fc16_u32 rmr_fc16_entry_data(rmr_fc16_context *c,const rmr_fc16_u8 *data,rmr_fc16_usize len){
    if(!c||(!data&&len))return RMR_FC16_ERR_ARGUMENT;if(!c->entry_open)return RMR_FC16_ERR_STATE;if(c->entry_seen_size+(rmr_fc16_u64)len>c->entry_declared_size)return RMR_FC16_ERR_SIZE;
    c->entry_crc_state=rmr_fc16_crc32c_update(c->entry_crc_state,data,len);rmr_fc16_sha256_update(&c->sha,data,len);
    rmr_fc16_usize i=0u;
    while(i<len && ((c->entry_seen_size+(rmr_fc16_u64)i)&15u)!=0u){rmr_fc16_u64 absolute=c->entry_seen_size+(rmr_fc16_u64)i;rmr_fc16_u32 lane=(c->entry_base_lane+(rmr_fc16_u32)(absolute>>4u))&15u;rmr_fc16_lane *l=&c->lane[lane];l->crc_state=rmr_fc16_crc32c_update(l->crc_state,data+i,1u);l->xor_fold^=(rmr_fc16_u32)data[i]<<((rmr_fc16_u32)(absolute&3u)*8u);l->bytes++;i++;}
    for(;i+16u<=len;i+=16u){rmr_fc16_u64 absolute=c->entry_seen_size+(rmr_fc16_u64)i;rmr_fc16_u32 lane=(c->entry_base_lane+(rmr_fc16_u32)(absolute>>4u))&15u;rmr_fc16_lane *l=&c->lane[lane];l->crc_state=rmr_fc16_crc32c_update(l->crc_state,data+i,16u);l->xor_fold^=rmr_fc16_xor_fold16(data+i);l->bytes+=16u;l->blocks16++;}
    for(;i<len;++i){rmr_fc16_u64 absolute=c->entry_seen_size+(rmr_fc16_u64)i;rmr_fc16_u32 lane=(c->entry_base_lane+(rmr_fc16_u32)(absolute>>4u))&15u;rmr_fc16_lane *l=&c->lane[lane];l->crc_state=rmr_fc16_crc32c_update(l->crc_state,data+i,1u);l->xor_fold^=(rmr_fc16_u32)data[i]<<((rmr_fc16_u32)(absolute&3u)*8u);l->bytes++;}
    c->entry_seen_size+=(rmr_fc16_u64)len;c->total_bytes+=(rmr_fc16_u64)len;return RMR_FC16_OK;
}

rmr_fc16_u32 rmr_fc16_entry_end(rmr_fc16_context *c){
    if(!c)return RMR_FC16_ERR_ARGUMENT;if(!c->entry_open)return RMR_FC16_ERR_STATE;if(c->entry_seen_size!=c->entry_declared_size)return RMR_FC16_ERR_SIZE;
    rmr_fc16_u8 end[24];end[0]='E';end[1]='N';end[2]='D';end[3]='R';put_u32(end+4,c->path_crc_final);put_u32(end+8,rmr_fc16_crc32c_finish(c->entry_crc_state));put_u64(end+12,c->entry_seen_size);put_u32(end+20,c->entry_base_lane);feed_meta(c,end,24u);c->entry_count++;c->entry_open=0u;return RMR_FC16_OK;
}

rmr_fc16_u32 rmr_fc16_finalize(rmr_fc16_context *c,rmr_fc16_u32 ta,rmr_fc16_snapshot *o){
    if(!c||!o)return RMR_FC16_ERR_ARGUMENT;if(c->entry_open)return RMR_FC16_ERR_STATE;c->thermal_after_milli_c=ta;
    rmr_fc16_u32 accepted=1u;if(c->thermal_limit_milli_c&&((c->thermal_before_milli_c>c->thermal_limit_milli_c)||(ta>c->thermal_limit_milli_c)))accepted=0u;rmr_fc16_u32 d=(ta>c->thermal_before_milli_c)?ta-c->thermal_before_milli_c:c->thermal_before_milli_c-ta;if(c->thermal_max_delta_milli_c&&d>c->thermal_max_delta_milli_c)accepted=0u;if(accepted)c->flags|=RMR_FC16_FLAG_THERMAL_ACCEPT;
    rmr_fc16_u8 final_meta[28];final_meta[0]='F';final_meta[1]='I';final_meta[2]='N';final_meta[3]='L';put_u64(final_meta+4,c->entry_count);put_u64(final_meta+12,c->total_bytes);put_u32(final_meta+20,c->thermal_before_milli_c);put_u32(final_meta+24,ta);feed_meta(c,final_meta,28u);
    zero_bytes(o,sizeof(*o));o->version=RMR_FC16_VERSION;o->flags=c->flags;o->root_crc32c=rmr_fc16_crc32c_finish(c->root_crc_state);o->path_count_low=(rmr_fc16_u32)c->entry_count;o->entry_count=c->entry_count;o->total_bytes=c->total_bytes;o->hw_stamp=c->hw_stamp;o->thermal_before_milli_c=c->thermal_before_milli_c;o->thermal_after_milli_c=ta;for(rmr_fc16_u32 i=0u;i<16u;++i){o->lane_crc32c[i]=rmr_fc16_crc32c_finish(c->lane[i].crc_state);o->lane_xor_fold[i]=c->lane[i].xor_fold;o->lane_bytes[i]=c->lane[i].bytes;}rmr_fc16_sha256_final(&c->sha,o->sha256);return accepted?RMR_FC16_OK:RMR_FC16_ERR_THERMAL;
}

rmr_fc16_u32 rmr_fc16_snapshot_equal(const rmr_fc16_snapshot *a,const rmr_fc16_snapshot *b){
    if(!a||!b)return 0u;rmr_fc16_u32 diff=0u;diff|=a->root_crc32c^b->root_crc32c;diff|=a->path_count_low^b->path_count_low;diff|=(rmr_fc16_u32)(a->entry_count^b->entry_count);diff|=(rmr_fc16_u32)((a->entry_count^b->entry_count)>>32u);diff|=(rmr_fc16_u32)(a->total_bytes^b->total_bytes);diff|=(rmr_fc16_u32)((a->total_bytes^b->total_bytes)>>32u);for(rmr_fc16_u32 i=0u;i<16u;++i){diff|=a->lane_crc32c[i]^b->lane_crc32c[i];diff|=a->lane_xor_fold[i]^b->lane_xor_fold[i];diff|=(rmr_fc16_u32)(a->lane_bytes[i]^b->lane_bytes[i]);diff|=(rmr_fc16_u32)((a->lane_bytes[i]^b->lane_bytes[i])>>32u);}for(rmr_fc16_u32 i=0u;i<32u;++i)diff|=a->sha256[i]^b->sha256[i];return diff==0u;
}

rmr_fc16_u32 rmr_fc16_delta64_build(const rmr_fc16_u64 *before,const rmr_fc16_u64 *after,rmr_fc16_usize n,rmr_fc16_delta64 *out,rmr_fc16_usize cap,rmr_fc16_usize *out_n){
    if(!before||!after||!out_n||(cap&& !out))return RMR_FC16_ERR_ARGUMENT;rmr_fc16_usize k=0u;for(rmr_fc16_usize i=0u;i<n;++i){rmr_fc16_u64 m=before[i]^after[i];if(m){if(k>=cap)return RMR_FC16_ERR_CAPACITY;out[k].word_index=i;out[k].xor_mask=m;k++;}}*out_n=k;return RMR_FC16_OK;
}
rmr_fc16_u32 rmr_fc16_delta64_apply(rmr_fc16_u64 *target,rmr_fc16_usize n,const rmr_fc16_delta64 *d,rmr_fc16_usize dn){if(!target||(!d&&dn))return RMR_FC16_ERR_ARGUMENT;for(rmr_fc16_usize i=0u;i<dn;++i){if(d[i].word_index>=n)return RMR_FC16_ERR_SIZE;target[d[i].word_index]^=d[i].xor_mask;}return RMR_FC16_OK;}

void rmr_fc16_warmup(rmr_fc16_u8 a[256],rmr_fc16_u8 b[256],rmr_fc16_u32 rounds){if(!a||!b)return;rmr_fc16_u8 m[16];for(rmr_fc16_u32 r=0u;r<rounds;++r)for(rmr_fc16_u32 lane=0u;lane<16u;++lane){rmr_fc16_u32 off=lane*16u;(void)rmr_fc16_diff16(a+off,b+off,m);rmr_fc16_apply16(a+off,m);rmr_fc16_apply16(a+off,m);a[off+(r&15u)]^=(rmr_fc16_u8)(lane+r);a[off+(r&15u)]^=(rmr_fc16_u8)(lane+r);}}

rmr_fc16_u32 rmr_fc16_median_u64(rmr_fc16_u64 *s,rmr_fc16_u32 n,rmr_fc16_u64 *out){if(!s||!out||n==0u||n>RMR_FC16_MAX_BENCH_SAMPLES)return RMR_FC16_ERR_ARGUMENT;for(rmr_fc16_u32 i=1u;i<n;++i){rmr_fc16_u64 key=s[i];rmr_fc16_u32 j=i;while(j&&s[j-1u]>key){s[j]=s[j-1u];j--;}s[j]=key;}*out=s[n>>1u];return RMR_FC16_OK;}
