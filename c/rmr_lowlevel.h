#ifndef RMR_LOWLEVEL_H
#define RMR_LOWLEVEL_H

#include <stddef.h>
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
#define RMR_TRAP() __builtin_trap()
#elif defined(_MSC_VER)
#include <intrin.h>
#define RMR_TRAP() __debugbreak()
#else
#define RMR_TRAP()                      \
  do {                                  \
    volatile uint8_t *p = (uint8_t *)0; \
    *p = 0;                             \
  } while (0)
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

static inline void rmr_memcpy(void *dst, const void *src, size_t len) {
  uint8_t *out = (uint8_t *)dst;
  const uint8_t *in = (const uint8_t *)src;
  size_t i = 0;
  if (((uintptr_t)out | (uintptr_t)in) % sizeof(size_t) == 0) {
    size_t *outw = (size_t *)out;
    const size_t *inw = (const size_t *)in;
    size_t words = len / sizeof(size_t);
    for (i = 0; i < words; i++) {
      outw[i] = inw[i];
    }
    i *= sizeof(size_t);
  }
  for (; i < len; i++) {
    out[i] = in[i];
  }
}

static inline void rmr_memset(void *dst, uint8_t value, size_t len) {
  uint8_t *out = (uint8_t *)dst;
  size_t i = 0;
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

#endif
