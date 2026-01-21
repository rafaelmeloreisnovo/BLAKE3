#ifndef RMR_LOWLEVEL_H
#define RMR_LOWLEVEL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "rmr_arch.h"
#if !defined(RMR_NO_LIBC)
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
#define RMR_TRAP() __builtin_trap()
#define RMR_INLINE static inline __attribute__((always_inline))
#define RMR_LIKELY(x) __builtin_expect(!!(x), 1)
#define RMR_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define RMR_RESTRICT __restrict__
#define RMR_ASSUME(x)            \
  do {                           \
    if (!(x)) {                  \
      __builtin_unreachable();   \
    }                            \
  } while (0)
#define RMR_PREFETCH_R(p) __builtin_prefetch((p), 0, 3)
#define RMR_PREFETCH_W(p) __builtin_prefetch((p), 1, 3)
#elif defined(_MSC_VER)
#include <intrin.h>
#define RMR_TRAP() __debugbreak()
#define RMR_INLINE static __forceinline
#define RMR_LIKELY(x) (x)
#define RMR_UNLIKELY(x) (x)
#define RMR_RESTRICT
#define RMR_ASSUME(x) __assume(x)
#define RMR_PREFETCH_R(p) (void)(p)
#define RMR_PREFETCH_W(p) (void)(p)
#else
#define RMR_TRAP()                      \
  do {                                  \
    volatile uint8_t *p = (uint8_t *)0; \
    *p = 0;                             \
  } while (0)
#define RMR_INLINE static inline
#define RMR_LIKELY(x) (x)
#define RMR_UNLIKELY(x) (x)
#define RMR_RESTRICT
#define RMR_ASSUME(x) (void)(x)
#define RMR_PREFETCH_R(p) (void)(p)
#define RMR_PREFETCH_W(p) (void)(p)
#endif

#if defined(BLAKE3_TESTING)
#define RMR_ASSERT(expr)      \
  do {                        \
    if (!(expr)) {            \
      RMR_TRAP();             \
    }                         \
  } while (0)
#else
#define RMR_ASSERT(expr) ((void)(expr))
#endif

RMR_INLINE void rmr_memcpy(void *RMR_RESTRICT dst,
                           const void *RMR_RESTRICT src,
                           size_t len) {
  if (RMR_UNLIKELY(len == 0)) {
    return;
  }
  uint8_t *RMR_RESTRICT out = (uint8_t *)dst;
  const uint8_t *RMR_RESTRICT in = (const uint8_t *)src;
  size_t i = 0;
  if (RMR_LIKELY(len <= 16)) {
    switch (len) {
      case 16: out[15] = in[15];
      case 15: out[14] = in[14];
      case 14: out[13] = in[13];
      case 13: out[12] = in[12];
      case 12: out[11] = in[11];
      case 11: out[10] = in[10];
      case 10: out[9] = in[9];
      case 9: out[8] = in[8];
      case 8: out[7] = in[7];
      case 7: out[6] = in[6];
      case 6: out[5] = in[5];
      case 5: out[4] = in[4];
      case 4: out[3] = in[3];
      case 3: out[2] = in[2];
      case 2: out[1] = in[1];
      case 1: out[0] = in[0];
      default: return;
    }
    return;
  }
  if (((uintptr_t)out | (uintptr_t)in) % sizeof(size_t) == 0) {
    size_t *outw = (size_t *)out;
    const size_t *inw = (const size_t *)in;
    size_t words = len / sizeof(size_t);
    const size_t prefetch_distance = 16;
    const size_t prefetch_stride = 8;
    RMR_ASSUME(prefetch_stride != 0);
    RMR_ASSUME((prefetch_stride & (prefetch_stride - 1)) == 0);
    const size_t prefetch_mask = prefetch_stride - 1;
    for (i = 0; i < words; i++) {
      if (RMR_LIKELY(words > prefetch_distance) &&
          (i & prefetch_mask) == 0) {
        RMR_PREFETCH_R(inw + i + prefetch_distance);
        RMR_PREFETCH_W(outw + i + prefetch_distance);
      }
      outw[i] = inw[i];
    }
    i *= sizeof(size_t);
  }
  for (; i < len; i++) {
    out[i] = in[i];
  }
}

RMR_INLINE void rmr_memset(void *RMR_RESTRICT dst, uint8_t value, size_t len) {
  if (RMR_UNLIKELY(len == 0)) {
    return;
  }
  uint8_t *RMR_RESTRICT out = (uint8_t *)dst;
  size_t i = 0;
  if (RMR_LIKELY(len <= 16)) {
    switch (len) {
      case 16: out[15] = value;
      case 15: out[14] = value;
      case 14: out[13] = value;
      case 13: out[12] = value;
      case 12: out[11] = value;
      case 11: out[10] = value;
      case 10: out[9] = value;
      case 9: out[8] = value;
      case 8: out[7] = value;
      case 7: out[6] = value;
      case 6: out[5] = value;
      case 5: out[4] = value;
      case 4: out[3] = value;
      case 3: out[2] = value;
      case 2: out[1] = value;
      case 1: out[0] = value;
      default: return;
    }
    return;
  }
  if (((uintptr_t)out) % sizeof(size_t) == 0) {
    size_t pattern = value;
    pattern |= pattern << 8;
    pattern |= pattern << 16;
#if SIZE_MAX > 0xffffffffu
    pattern |= pattern << 32;
#endif
    size_t *outw = (size_t *)out;
    size_t words = len / sizeof(size_t);
    for (i = 0; i < words; i++) {
      outw[i] = pattern;
    }
    i *= sizeof(size_t);
  }
  for (; i < len; i++) {
    out[i] = value;
  }
}

#if defined(RMR_NO_LIBC)
size_t rmr_ll_strlen(const char *value);
int rmr_ll_strcmp(const char *left, const char *right);
const char *rmr_ll_strerror(int errnum);
void *rmr_ll_malloc(size_t size);
void rmr_ll_free(void *ptr);
unsigned long long rmr_ll_strtoull(const char *text, char **end, int base);
#else
RMR_INLINE size_t rmr_ll_strlen(const char *value) { return strlen(value); }

RMR_INLINE int rmr_ll_strcmp(const char *left, const char *right) {
  return strcmp(left, right);
}

RMR_INLINE const char *rmr_ll_strerror(int errnum) {
  return strerror(errnum);
}

RMR_INLINE void *rmr_ll_malloc(size_t size) { return malloc(size); }

RMR_INLINE void rmr_ll_free(void *ptr) { free(ptr); }
#endif

RMR_INLINE bool rmr_ll_parse_size(const char *text, size_t *out) {
  char *end = NULL;
#if defined(RMR_NO_LIBC)
  unsigned long long value = rmr_ll_strtoull(text, &end, 10);
  if (RMR_UNLIKELY(end == text || *end != '\0' || value > SIZE_MAX)) {
    return false;
  }
#else
  errno = 0;
  unsigned long long value = strtoull(text, &end, 10);
  if (RMR_UNLIKELY(errno != 0 || end == text || *end != '\0' || value > SIZE_MAX)) {
    return false;
  }
#endif
  *out = (size_t)value;
  return true;
}

#endif
