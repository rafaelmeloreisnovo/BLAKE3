#ifndef RMR_PV1_ASSERT_H
#define RMR_PV1_ASSERT_H
#if defined(NDEBUG)
#define assert(x) ((void)0)
#else
#define assert(x) ((void)sizeof(x))
#endif
#endif
