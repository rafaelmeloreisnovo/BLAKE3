// SPDX-License-Identifier: Apache-2.0 OR CC0-1.0
#include "rmr_stability.h"
#include <limits.h>

#if defined(RMR_HAS_STABILITY_ARMV7) && RMR_HAS_STABILITY_ARMV7
extern uint32_t rmr_stability_step_armv7(uint32_t state_words[4], const uint8_t *events, uint32_t count);
#endif

static uint32_t rotl32(uint32_t x,uint32_t n){return(x<<n)|(x>>(32u-n));}
static uint32_t rotr32(uint32_t x,uint32_t n){return(x>>n)|(x<<(32u-n));}
static uint32_t absdiff32(uint32_t a,uint32_t b){return a>=b?a-b:b-a;}
static uint32_t satadd32(uint32_t a,uint32_t b,uint32_t *st){if(UINT32_MAX-a<b){if(st)*st|=RMR_STABILITY_STATUS_COUNTER_SATURATED;return UINT32_MAX;}return a+b;}
static uint32_t ratio_q30(uint32_t n,uint32_t d){return d?(uint32_t)(((uint64_t)n<<30u)/d):0u;}
static uint32_t ratio_q16(uint32_t n,uint32_t d){return d?(uint32_t)(((uint64_t)n<<16u)/d):0u;}

static uint32_t crc32c_update(uint32_t crc,const uint8_t *p,size_t n){
    while(n--){crc^=*p++;for(uint32_t i=0;i<8u;i++){uint32_t m=(uint32_t)-(int32_t)(crc&1u);crc=(crc>>1u)^(0x82F63B78u&m);}}
    return crc;
}
uint32_t RmR_Stability_CRC32C(const void *data,size_t size){if(!data&&size)return 0u;return crc32c_update(0xFFFFFFFFu,(const uint8_t*)data,size)^0xFFFFFFFFu;}

void RmR_StabilityState_Init(RmR_StabilityState *s,uint32_t seed){
    if(!s)return;uint32_t x=seed?seed:0x963u;s->s0=x^0x243F6A88u;s->s1=rotl32(x,7u)^0x85A308D3u;s->s2=rotl32(x,13u)^0x13198A2Eu;s->s3=rotl32(x,21u)^0x03707344u;
}
uint32_t RmR_StabilityStepPortable(RmR_StabilityState *s,const uint8_t *e,size_t n){
    if(!s||(!e&&n))return 0u;uint32_t a=s->s0,b=s->s1,c=s->s2,d=s->s3;
    for(size_t i=0;i<n;i++){a^=b;b+=e[i];c=rotr32(c,5u);c+=a;d^=c;a+=d;}
    s->s0=a;s->s1=b;s->s2=c;s->s3=d;return(((a^b)+c)^d)&0x7FFFFFFFu;
}
uint32_t RmR_StabilityStep(RmR_StabilityState *s,const uint8_t *e,size_t n){
    if(!s||(!e&&n))return 0u;
#if defined(RMR_HAS_STABILITY_ARMV7) && RMR_HAS_STABILITY_ARMV7
    if(n<=UINT32_MAX)return rmr_stability_step_armv7(&s->s0,e,(uint32_t)n);
#endif
    return RmR_StabilityStepPortable(s,e,n);
}

void RmR_StabilityTrace_Init(RmR_StabilityTrace *t){if(!t)return;*t=(RmR_StabilityTrace){0};t->status=RMR_STABILITY_STATUS_NO_SAMPLES|RMR_STABILITY_STATUS_NO_PEAK_SAMPLES|RMR_STABILITY_STATUS_NO_NONPEAK_SAMPLES;}
void RmR_StabilityTrace_Add(RmR_StabilityTrace *t,uint32_t gate,uint8_t stable,uint8_t peak_flag){
    if(!t)return;uint32_t peak=peak_flag||gate==3u||gate==4u||gate==8u;t->rows=satadd32(t->rows,1u,&t->status);
    if(peak){t->peak_total=satadd32(t->peak_total,1u,&t->status);if(stable)t->peak_stable=satadd32(t->peak_stable,1u,&t->status);}
    else{t->nonpeak_total=satadd32(t->nonpeak_total,1u,&t->status);if(stable)t->nonpeak_stable=satadd32(t->nonpeak_stable,1u,&t->status);}
}
uint32_t RmR_StabilityTrace_Finalize(RmR_StabilityTrace *t){
    if(!t)return RMR_STABILITY_STATUS_BAD_ARGUMENT;t->status&=RMR_STABILITY_STATUS_COUNTER_SATURATED;
    if(!t->rows)t->status|=RMR_STABILITY_STATUS_NO_SAMPLES;if(!t->peak_total)t->status|=RMR_STABILITY_STATUS_NO_PEAK_SAMPLES;if(!t->nonpeak_total)t->status|=RMR_STABILITY_STATUS_NO_NONPEAK_SAMPLES;
    t->peak_rate_q30=ratio_q30(t->peak_stable,t->peak_total);t->nonpeak_rate_q30=ratio_q30(t->nonpeak_stable,t->nonpeak_total);t->delta_p_q30=(int32_t)t->peak_rate_q30-(int32_t)t->nonpeak_rate_q30;return t->status;
}

uint8_t RmR_Vision_OtsuThreshold(const uint8_t *g,uint32_t w,uint32_t h,uint32_t stride,uint32_t *status){
    if(status)*status=0u;if(!g||!w||!h||stride<w){if(status)*status|=RMR_STABILITY_STATUS_BAD_ARGUMENT;return 0u;}
    uint64_t total=(uint64_t)w*h;if(total>UINT32_MAX){if(status)*status|=RMR_STABILITY_STATUS_COUNTER_SATURATED;return 0u;}
    uint32_t hist[256]={0};uint64_t sum=0;
    for(uint32_t y=0;y<h;y++){const uint8_t *r=g+(size_t)y*stride;for(uint32_t x=0;x<w;x++){hist[r[x]]++;sum+=r[x];}}
    uint32_t shift=0;while((total>>shift)>65535u)shift++;
    uint64_t wb=0,sb=0,best=0;uint8_t threshold=0;
    for(uint32_t i=0;i<256;i++){
        wb+=hist[i];if(!wb)continue;uint64_t wf=total-wb;if(!wf)break;sb+=(uint64_t)i*hist[i];
        uint64_t mb=(sb<<16u)/wb,mf=((sum-sb)<<16u)/wf,d=(mb>=mf?mb-mf:mf-mb)>>8u;
        uint64_t swb=wb>>shift,swf=wf>>shift;if(!swb)swb=1;if(!swf)swf=1;uint64_t score=(d*d)*swb*swf;
        if(score>best){best=score;threshold=(uint8_t)i;}
    }
    return threshold;
}

static uint32_t angle_bin(int16_t deg){int32_t a=deg;while(a<0)a+=360;while(a>=360)a-=360;return(uint32_t)((a+22)/45)&7u;}
static uint32_t angular_chi2_q16(const uint32_t hist[8],uint32_t n,uint32_t *status){
    if(!n)return 0u;if(n>1000000u){if(status)*status|=RMR_STABILITY_STATUS_ANGLE_SATURATED;n=1000000u;}
    uint64_t num=0;for(uint32_t i=0;i<8;i++){int64_t d=(int64_t)(8u*hist[i])-(int64_t)n;num+=(uint64_t)(d*d);}uint64_t den=(uint64_t)8u*n,whole=num/den,rem=num%den;
    if(whole>(UINT32_MAX>>16u))return UINT32_MAX;uint64_t q=(whole<<16u)+((rem<<16u)/den);return(uint32_t)(q>UINT32_MAX?UINT32_MAX:q);
}
static uint32_t angular_concentration_q16(const uint32_t hist[8],uint32_t n){
    if(!n)return 0u;uint32_t m=hist[0];for(uint32_t i=1;i<8;i++)if(hist[i]>m)m=hist[i];uint64_t s=(uint64_t)m*8u;if(s<=n)return 0u;return(uint32_t)(((s-n)<<16u)/((uint64_t)7u*n));
}
static void crc_u32(uint32_t *crc,uint32_t v){uint8_t b[4]={(uint8_t)v,(uint8_t)(v>>8u),(uint8_t)(v>>16u),(uint8_t)(v>>24u)};*crc=crc32c_update(*crc,b,4u);}

uint32_t RmR_Vision_BuildDescriptor(const uint8_t *g,uint32_t w,uint32_t h,uint32_t stride,const int16_t *angles,uint32_t angle_count,RmR_VisionDescriptor *o){
    if(!o)return RMR_STABILITY_STATUS_BAD_ARGUMENT;*o=(RmR_VisionDescriptor){0};o->width=w;o->height=h;
    if(!g||!w||!h||stride<w||(!angles&&angle_count)){o->status=RMR_STABILITY_STATUS_BAD_ARGUMENT;return o->status;}
    uint64_t pc=(uint64_t)w*h;if(pc>UINT32_MAX){o->status=RMR_STABILITY_STATUS_COUNTER_SATURATED;return o->status;}o->pixel_count=(uint32_t)pc;o->otsu_threshold=RmR_Vision_OtsuThreshold(g,w,h,stride,&o->status);
    uint32_t gray_crc=0xFFFFFFFFu;for(uint32_t y=0;y<h;y++){const uint8_t *r=g+(size_t)y*stride;gray_crc=crc32c_update(gray_crc,r,w);for(uint32_t x=0;x<w;x++)if(r[x]<=o->otsu_threshold)o->foreground_count++;}
    o->gray_crc32c=gray_crc^0xFFFFFFFFu;o->foreground_q16=ratio_q16(o->foreground_count,o->pixel_count);
    uint32_t used=angle_count>1000000u?1000000u:angle_count;if(used!=angle_count)o->status|=RMR_STABILITY_STATUS_ANGLE_SATURATED;o->angle_count=used;
    for(uint32_t i=0;i<used;i++){uint32_t b=angle_bin(angles[i]);o->angle_hist[b]=satadd32(o->angle_hist[b],1u,&o->status);}o->angular_chi2_q16=angular_chi2_q16(o->angle_hist,used,&o->status);o->angular_concentration_q16=angular_concentration_q16(o->angle_hist,used);
    uint32_t crc=0xFFFFFFFFu;crc_u32(&crc,o->width);crc_u32(&crc,o->height);crc_u32(&crc,o->pixel_count);crc_u32(&crc,o->foreground_count);crc_u32(&crc,o->foreground_q16);crc_u32(&crc,o->angle_count);for(uint32_t i=0;i<8;i++)crc_u32(&crc,o->angle_hist[i]);crc_u32(&crc,o->angular_chi2_q16);crc_u32(&crc,o->angular_concentration_q16);crc_u32(&crc,o->gray_crc32c);crc=crc32c_update(crc,&o->otsu_threshold,1u);o->descriptor_crc32c=crc^0xFFFFFFFFu;return o->status;
}

uint32_t RmR_Vision_DifferenceQ16(const RmR_VisionDescriptor *a,const RmR_VisionDescriptor *b){
    if(!a||!b)return UINT32_MAX;uint32_t th=(absdiff32(a->otsu_threshold,b->otsu_threshold)<<16u)/255u,fg=absdiff32(a->foreground_q16,b->foreground_q16);uint64_t tv=0;
    if(a->angle_count||b->angle_count){for(uint32_t i=0;i<8;i++)tv+=absdiff32(ratio_q16(a->angle_hist[i],a->angle_count),ratio_q16(b->angle_hist[i],b->angle_count));tv>>=1u;if(tv>RMR_STABILITY_Q16_ONE)tv=RMR_STABILITY_Q16_ONE;}
    return(uint32_t)(((uint64_t)th+fg+tv)/3u);
}
uint32_t RmR_Stability_DifferenceHash(const RmR_VisionDescriptor *a,const RmR_VisionDescriptor *b){
    if(!a||!b)return 0u;uint32_t crc=0xFFFFFFFFu,v[]={a->descriptor_crc32c^b->descriptor_crc32c,a->gray_crc32c^b->gray_crc32c,absdiff32(a->foreground_q16,b->foreground_q16),absdiff32(a->angular_chi2_q16,b->angular_chi2_q16),absdiff32(a->angular_concentration_q16,b->angular_concentration_q16),RmR_Vision_DifferenceQ16(a,b)};
    for(size_t i=0;i<sizeof(v)/sizeof(v[0]);i++)crc_u32(&crc,v[i]);for(uint32_t i=0;i<8;i++)crc_u32(&crc,a->angle_hist[i]^b->angle_hist[i]);return crc^0xFFFFFFFFu;
}
