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
  for (size_t i = 0; i < len; i++) {
    out[i] = in[i];
  }
}

static inline void rmr_memset(void *dst, uint8_t value, size_t len) {
  uint8_t *out = (uint8_t *)dst;
  for (size_t i = 0; i < len; i++) {
    out[i] = value;
  }
}

#endif
